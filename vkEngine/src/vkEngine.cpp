#include <cmath>
#include <functional>
#define VMA_IMPLEMENTATION
#include "vkEngine/vkEngine.hpp"
#include "vkEngine/infoCreator.hpp"
#include "vkEngine/pipeline_builder.hpp"
#include "vkEngine/utils.hpp"
#include "vkEngine/DescriptorLayoutBuilder.hpp"
#include "vkEngine/DescriptorWriter.hpp"
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <array>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <vector>

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
#ifndef DYNAMIC_LOADER_LOADED
#define DYNAMIC_LOADER_LOADED
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif
#endif

mvk::vkEngine *mvk::vkEngine::m_engine = nullptr;

mvk::vkEngine& mvk::vkEngine::get(SDL_Window *window) {
  if (!m_engine) {
    m_engine = new mvk::vkEngine(window);
  }
  return *m_engine;
}

void mvk::vkEngine::init() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif // VULKAN_HPP_DISPATCH_LOADER_DYNAMIC

  this->_initVulkan();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->m_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->m_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
  //
  int width, height;
  SDL_GetWindowSize(this->m_window, &width, &height);

  vma::VulkanFunctions vulkanFunctions{
      .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = &vkGetDeviceProcAddr
  };
  vma::AllocatorCreateInfo allocatorInfo{
      .flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress,
      .physicalDevice = this->m_physDevice,
      .device = this->m_device,
      .pVulkanFunctions = &vulkanFunctions,
      .instance = this->m_instance,
      .vulkanApiVersion = vk::ApiVersion13,
  };
  this->m_allocator = vma::createAllocator(allocatorInfo);
  this->m_deletionStack.pushFunction([&]() { 
    std::cout << "Destroying Allocator" << std::endl;
    this->m_allocator.destroy(); 
  });

  this->_initSwapchain(static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height));
  this->_initCommandPool(this->m_graphicsQueueIndex);
  this->_allocateCommandBuffer(this->m_graphicsCommandBuffer);

  this->_initDescriptors();
  this->_updateDescriptorSet(this->m_descriptorSet);
  this->_updateUbos(this->m_ubo);
  std::vector<vk::DescriptorSetLayout> layouts = {this->m_descriptorSet.layout};
  this->_initGraphicPipeline(layouts);
  this->_initSynchronizationObjects();


  this->m_deletionStack.pushFunction([&]() {
    for (const auto &buffer : testMeshes) {
      std::cout << "Destroying mesh buffers for " << buffer->name << std::endl;
      this->_destroyBuffer(buffer->buffers.vertexBuffer);
      this->_destroyBuffer(buffer->buffers.indexBuffer);
    }
  });
}

void mvk::vkEngine::draw() {
  this->m_device.waitForFences(this->m_inFlightFence, vk::True, UINT64_MAX);
  this->m_device.resetFences(this->m_inFlightFence);

  uint32_t imageIndex =
      this->m_device
          .acquireNextImageKHR(this->m_graphicSwapchain.swapchain, UINT64_MAX,
                               this->m_imageAvailableSempahore)
          .value;

  this->m_graphicsCommandBuffer.reset();

  this->_recordCommandBuffer(this->m_graphicsCommandBuffer, imageIndex);

  auto cmdSubmitInfo =
      utils::commandBufferSubmitInfo(this->m_graphicsCommandBuffer);

  auto signalInfo = utils::semaphoreSubmitInfo(
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      this->m_renderFinishedSemaphore);
  auto waitInfo = utils::semaphoreSubmitInfo(
      vk::PipelineStageFlagBits2::eAllGraphics, this->m_imageAvailableSempahore);

  auto submitInfo2 = utils::submitInfo(&cmdSubmitInfo, &signalInfo, &waitInfo);
  this->m_graphicsQueue.submit2(submitInfo2, this->m_inFlightFence);

  vk::PresentInfoKHR prensetInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &this->m_renderFinishedSemaphore,
      .swapchainCount = 1,
      .pSwapchains = &this->m_graphicSwapchain.swapchain,
      .pImageIndices = &imageIndex};
  this->m_graphicsQueue.presentKHR(prensetInfo);
}

void mvk::vkEngine::cleanup() {
  this->m_device.waitIdle();
  this->m_deletionStack.flush();
}

mvk::vkEngine::vkEngine(SDL_Window *window) : m_window(window) {}

void mvk::vkEngine::_allocateCommandBuffer(vk::CommandBuffer &buffer) {
  vk::CommandBufferAllocateInfo bufferAllocationInfo{
      .commandPool = this->m_graphicsCommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1};
  buffer = this->m_device.allocateCommandBuffers(bufferAllocationInfo).front();

  vk::CommandBufferAllocateInfo immediateAllocationInfo{
      .commandPool = this->m_immediateCommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1,
  };
  this->m_immediateCommandBuffer =
      this->m_device.allocateCommandBuffers(immediateAllocationInfo).front();
}

void mvk::vkEngine::_initVulkan() {
  vkb::InstanceBuilder instanceBuilder;

  auto instRet = instanceBuilder.set_app_name("My Vulkan App")
                     .require_api_version(1, 3, 0)
#ifndef NDEBUG
                     .request_validation_layers(true)
                     .use_default_debug_messenger()
#endif // !NDEBUG
                     .build();

  this->m_instance = vk::Instance(instRet.value().instance);

  this->m_deletionStack.pushFunction([=, this]() { this->m_instance.destroy(); });

#ifndef NDEBUG
  this->m_debugMessanger = vk::DebugUtilsMessengerEXT(instRet->debug_messenger);
  this->m_deletionStack.pushFunction([=, this]() {
    std::cout << "Destroying debug messenger\n";
    this->m_instance.destroyDebugUtilsMessengerEXT(this->m_debugMessanger);
  });
#endif // !NDEBUG

  VkSurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(this->m_window,
                                static_cast<VkInstance>(this->m_instance),
                                nullptr, &surface)) {
    std::cerr << "Error generating surface\n";
  }
  this->m_surface = vk::SurfaceKHR(surface);

  this->m_deletionStack.pushFunction(
      [=, this]() { this->m_instance.destroySurfaceKHR(this->m_surface); });

  vkb::PhysicalDeviceSelector selector{instRet.value()};
  vk::PhysicalDeviceVulkan13Features features_13{
      .synchronization2 = true,
      .dynamicRendering = true,
  };
  vk::PhysicalDeviceVulkan12Features features_12{
      .bufferDeviceAddress = true,
  };
  vkb::PhysicalDevice vkbPhysDevice =
      selector.set_minimum_version(1, 3)
          .set_surface(static_cast<VkSurfaceKHR>(this->m_surface))
          .set_required_features_12(
              static_cast<VkPhysicalDeviceVulkan12Features>(features_12))
          .set_required_features_13(
              static_cast<VkPhysicalDeviceVulkan13Features>(features_13))
          .select()
          .value();

  vkb::DeviceBuilder deviceBuilder{vkbPhysDevice};

  vkb::Device vkbDevice = deviceBuilder.build().value();

  this->m_device = vk::Device(vkbDevice.device);
  this->m_physDevice = vk::PhysicalDevice(vkbPhysDevice.physical_device);
  this->m_graphicsQueue =
      vk::Queue(vkbDevice.get_queue(vkb::QueueType::graphics).value());
  this->m_graphicsQueueIndex =
      vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
  this->m_deletionStack.pushFunction([=, this]() { 
    std::cout << "Destroying device\n";
    this->m_device.destroy(); 
  });
}

void mvk::vkEngine::_initCommandPool(uint32_t queueFamilyIndex) {
  vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = (uint32_t)queueFamilyIndex};

  this->m_graphicsCommandPool = this->m_device.createCommandPool(poolInfo);
  this->m_immediateCommandPool = this->m_device.createCommandPool(poolInfo);

  this->m_deletionStack.pushFunction([&]() {
    std::cout << "Destroying command pools\n";
    this->m_device.destroyCommandPool(this->m_graphicsCommandPool);
    this->m_device.destroyCommandPool(this->m_immediateCommandPool);
  });
}

void mvk::vkEngine::_initSwapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapchainBuilder{
      static_cast<VkPhysicalDevice>(this->m_physDevice),
      static_cast<VkDevice>(this->m_device),
      static_cast<VkSurfaceKHR>(this->m_surface)};

  vk::ImageUsageFlags swapchainUsageFlags =
      vk::ImageUsageFlagBits::eColorAttachment |
      vk::ImageUsageFlagBits::eTransferDst;

  vkb::Swapchain vkbSwapchain =
      swapchainBuilder
          .use_default_format_selection()
          // use vsync present mode
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
          .set_desired_extent(width, height)
          .set_image_usage_flags(
              static_cast<VkImageUsageFlags>(swapchainUsageFlags))
          .build()
          .value();

  // store swapchain and its related images
  this->m_graphicSwapchain.swapchain = vkbSwapchain.swapchain;
  auto _swapchainImages = vkbSwapchain.get_images().value();
  auto _swapchainImageViews = vkbSwapchain.get_image_views().value();

  this->m_graphicSwapchain.images.reserve(_swapchainImages.size());
  for (auto image : _swapchainImages) {
    this->m_graphicSwapchain.images.emplace_back(vk::Image(image));
  }

  this->m_graphicSwapchain.imageViews.reserve(_swapchainImageViews.size());
  for (auto image : _swapchainImageViews) {
    this->m_graphicSwapchain.imageViews.emplace_back(vk::ImageView(image));
  }

  this->m_graphicSwapchain.format = vk::Format(vkbSwapchain.image_format);

  this->m_swapchainExtent = vkbSwapchain.extent;

  // Creating Depth testing image
  this->m_depthImage.format = vk::Format::eD32Sfloat;
  this->m_depthImage.extent.width = width;
  this->m_depthImage.extent.height = height;
  this->m_depthImage.extent.depth = 1;

  vk::ImageCreateInfo imageInfo = mvk::utils::imageCreateInfo(
      this->m_depthImage.format, vk::ImageUsageFlagBits::eDepthStencilAttachment,
      this->m_depthImage.extent);

  vma::AllocationCreateInfo allocCreateInfo{
      .usage = vma::MemoryUsage::eAuto,
  };

  auto allocatedImage =
      this->m_allocator.createImage(imageInfo, allocCreateInfo);
  this->m_depthImage.image = allocatedImage.first;
  this->m_depthImage.allocation = allocatedImage.second;

  vk::ImageViewCreateInfo viewInfo = mvk::utils::imageViewCreateInfo(
      this->m_depthImage.format, this->m_depthImage.image,
      vk::ImageAspectFlagBits::eDepth);

  this->m_depthImage.imageView = this->m_device.createImageView(viewInfo);

  this->_createDrawImage();

  this->m_deletionStack.pushFunction([=, this]() {
    std::cout << "Destroying swapchain and its images\n";
    this->m_device.destroySwapchainKHR(this->m_graphicSwapchain.swapchain);

    for (unsigned long i = 0; i < this->m_graphicSwapchain.imageViews.size();
         i++) {
      this->m_device.destroyImageView(this->m_graphicSwapchain.imageViews[i]);
    }

    this->m_device.destroyImageView(this->m_depthImage.imageView);
    this->m_allocator.destroyImage(this->m_depthImage.image,
                                  this->m_depthImage.allocation);
  });
}

void mvk::vkEngine::_createDrawImage() {
  m_drawImage.extent = {
      this->m_swapchainExtent.width,
      this->m_swapchainExtent.height,
      1,
  };

  m_drawImage.format = vk::Format::eR16G16B16A16Sfloat;

  vk::ImageUsageFlags drawImageFlags = vk::ImageUsageFlagBits::eTransferSrc |
                                       vk::ImageUsageFlagBits::eTransferSrc |
                                       vk::ImageUsageFlagBits::eStorage |
                                       vk::ImageUsageFlagBits::eColorAttachment;

  vk::ImageCreateInfo drawImageInfo = mvk::utils::imageCreateInfo(
      this->m_drawImage.format, drawImageFlags, m_drawImage.extent);

  vma::AllocationCreateInfo imageAllocInfo{
      .usage = vma::MemoryUsage::eGpuOnly,
      .requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal};

  auto imageAllocation =
      this->m_allocator.createImage(drawImageInfo, imageAllocInfo);
  m_drawImage.image = imageAllocation.first;
  m_drawImage.allocation = imageAllocation.second;

  auto imageViewInfo = mvk::utils::imageViewCreateInfo(
      m_drawImage.format, m_drawImage.image, vk::ImageAspectFlagBits::eColor);
  this->m_drawImage.imageView = this->m_device.createImageView(imageViewInfo);

  this->m_deletionStack.pushFunction([&]() {
    std::cout << "Destroying draw Image\n";
    this->m_device.destroyImageView(this->m_drawImage.imageView);
    this->m_allocator.destroyImage(this->m_drawImage.image,
                                  this->m_drawImage.allocation);
  });
}

void mvk::vkEngine::_initGraphicPipeline(
    std::span<vk::DescriptorSetLayout> layouts) {
  auto vertShaderCode =
      utils::readFile("resources/shaders/compiled/base_vertex.vert.spv");
  auto fragShaderCode =
      utils::readFile("resources/shaders/compiled/base_frag.frag.spv");

  auto vertModule = utils::createShaderModule(vertShaderCode, this->m_device);
  auto fragModule = utils::createShaderModule(fragShaderCode, this->m_device);

  vk::PushConstantRange pushConstantRanges{};
  pushConstantRanges.setOffset(0);
  pushConstantRanges.setSize(sizeof(GPUDrawPushConstants));
  pushConstantRanges.setStageFlags(vk::ShaderStageFlagBits::eVertex);

  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.setSetLayouts(layouts);
  pipelineLayoutCreateInfo.setPushConstantRanges(pushConstantRanges);
  this->m_graphicsPipelineLayout =
      this->m_device.createPipelineLayout(pipelineLayoutCreateInfo);

  PipelineBuilder pipelineBuilder;

  pipelineBuilder._pipelineLayout = m_graphicsPipelineLayout;
  pipelineBuilder.setShaders(vertModule, fragModule);
  pipelineBuilder.setInputTopology(vk::PrimitiveTopology::eTriangleList);
  pipelineBuilder.setPolygonMode(vk::PolygonMode::eFill);
  pipelineBuilder.setCullMode(vk::CullModeFlagBits::eNone,
                              vk::FrontFace::eClockwise);
  pipelineBuilder.setMultisamplingNone();
  pipelineBuilder.disableBlending();
  pipelineBuilder.enableDepthtest(true, vk::CompareOp::eGreaterOrEqual);
  pipelineBuilder.setColorAttachmentFormat(this->m_drawImage.format);
  pipelineBuilder.setDepthFormat(m_depthImage.format);

  m_graphicsPipeline = pipelineBuilder.buildPipeline(this->m_device);

  this->m_device.destroyShaderModule(vertModule);
  this->m_device.destroyShaderModule(fragModule);

  this->m_deletionStack.pushFunction([&]() {
    std::cout << "Destroying pipeline\n";
    this->m_device.destroyPipelineLayout(this->m_graphicsPipelineLayout);
    // this->_device.destroyRenderPass(this->_renderPass);
    this->m_device.destroyPipeline(this->m_graphicsPipeline);
  });
}

void mvk::vkEngine::_initFrameBuffers() {
  this->m_graphicSwapchain.frameBuffers.resize(
      this->m_graphicSwapchain.imageViews.size());

  for (size_t i = 0; i < m_graphicSwapchain.imageViews.size(); i++) {
    vk::ImageView attachments[] = {m_graphicSwapchain.imageViews[i]};

    vk::FramebufferCreateInfo framebufferInfo{
        .renderPass = this->m_renderPass,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = this->m_swapchainExtent.width,
        .height = this->m_swapchainExtent.height,
        .layers = 1};

    m_graphicSwapchain.frameBuffers[i] =
        this->m_device.createFramebuffer(framebufferInfo);
  }
  this->m_deletionStack.pushFunction([&]() {
    for (auto frameBuffer : m_graphicSwapchain.frameBuffers)
      this->m_device.destroyFramebuffer(frameBuffer);
  });
}

void mvk::vkEngine::_updateDescriptorSet(mvk::DescriptorObject &descriptor) {
  DescriptorWriter writer;
  writer.writeBuffer(0, descriptor.buffer.buffer, vk::WholeSize, descriptor.buffer.allocationInfo.offset, vk::DescriptorType::eUniformBuffer);
  writer.updateSet(this->m_device, descriptor.descriptor);
}

void mvk::vkEngine::_updateUbos(mvk::UniformDescriptorObject ubo) {
  static float rotationAngle;
  ubo.model = glm::identity<glm::mat4>();
  ubo.model =
      glm::rotate(ubo.model, glm::radians(rotationAngle++), glm::vec3(0, 1, 0));

  ubo.proj = glm::perspectiveRH_ZO(glm::radians(45.f),
                                   (float)this->m_drawImage.extent.width /
                                       (float)this->m_drawImage.extent.height,
                                   1000.f, 0.1f);
  ubo.proj[1][1] *= -1;

  ubo.view = glm::identity<glm::mat4>();
  ubo.view = glm::translate(ubo.view, glm::vec3(0, 1, -5));
  glm::vec3 cameraPos(ubo.view[3][0], ubo.view[3][1], ubo.view[3][2]);

  auto lookAtMat = glm::lookAtRH(cameraPos, glm::vec3(), glm::vec3(0, 1, 0));
  ubo.view = lookAtMat;
  /*{ // ROTATE AROUND*/
  /*  ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, 1));*/
  /*  ubo.view = glm::rotate(ubo.view, glm::radians(rotationAngle++),*/
  /*                         glm::vec3(0, 1, 0));*/
  /*  ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -1));*/
  /*}*/

  memcpy(this->m_descriptorSet.buffer.allocationInfo.pMappedData, &ubo,
         sizeof(ubo));
}

void mvk::vkEngine::_recordCommandBuffer(vk::CommandBuffer commandBuffer,
                                         uint32_t imageIndex) {
  vk::CommandBufferBeginInfo beginInfo{};
  commandBuffer.begin(beginInfo);

  utils::transitionImage(commandBuffer, this->m_depthImage.image,
                         vk::ImageLayout::eUndefined,
                         vk::ImageLayout::eDepthAttachmentOptimal);
  utils::transitionImage(commandBuffer, this->m_drawImage.image,
                         vk::ImageLayout::eUndefined,
                         vk::ImageLayout::eColorAttachmentOptimal);
  utils::transitionImage(
      commandBuffer, this->m_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
  vk::RenderingAttachmentInfo depthAttachment{
      .imageView = m_depthImage.imageView,
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.depthStencil = {.depth = 0.f}}};

  std::array<float, 4> clearColor = {0.f};
  vk::RenderingAttachmentInfo colorAttachment{
      .imageView = this->m_drawImage.imageView,
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.color = {clearColor}}};

  vk::RenderingInfo renderInfo{
      .renderArea = {.extent = {.width = this->m_drawImage.extent.width,
                                .height = this->m_drawImage.extent.height}},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachment,
      .pDepthAttachment = &depthAttachment,
  };

  commandBuffer.beginRendering(renderInfo);

  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             this->m_graphicsPipeline);

  vk::Viewport viewport{.x = 0.f,
                        .y = 0.f,
                        .width = (float)this->m_drawImage.extent.width,
                        .height = (float)this->m_drawImage.extent.height,
                        .minDepth = 0.f,
                        .maxDepth = 1.f};
  commandBuffer.setViewport(0, viewport);

  vk::Rect2D scissors{.offset = {0, 0}, .extent = this->m_swapchainExtent};

  commandBuffer.setScissor(0, scissors);

  this->_updateDescriptorSet(this->m_descriptorSet);
  this->_updateUbos(this->m_ubo);

  vk::DeviceSize offsets[] = {0};
  /*commandBuffer.bindVertexBuffers(0, this->tmpMesh.vertexBuffer.buffer,*/
  /*                                offsets);*/

  commandBuffer.bindIndexBuffer(this->testMeshes[2]->buffers.indexBuffer.buffer,
                                offsets[0], vk::IndexType::eUint32);

  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   this->m_graphicsPipelineLayout, 0,
                                   this->m_descriptorSet.descriptor, nullptr);
  /*commandBuffer.draw(3, 1, 0, 0);*/

  this->m_samplePushConstants.vertexBuffer =
      this->testMeshes[2]->buffers.vertexBufferAddress;
  commandBuffer.pushConstants<GPUDrawPushConstants>(
      this->m_graphicsPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,
      this->m_samplePushConstants);
  commandBuffer.drawIndexed(this->testMeshes[2]->buffers.indexCount, 1, 0, 0,
                            0);

  commandBuffer.endRendering();

  mvk::utils::transitionImage(this->m_graphicsCommandBuffer,
                              this->m_drawImage.image,
                              vk::ImageLayout::eColorAttachmentOptimal,
                              vk::ImageLayout::eTransferSrcOptimal);
  mvk::utils::transitionImage(
      this->m_graphicsCommandBuffer, this->m_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferDstOptimal);

  vk::Extent2D drawImageExtent{this->m_drawImage.extent.width,
                               this->m_drawImage.extent.height};
  utils::copyImageToImage(commandBuffer, this->m_drawImage.image,
                          this->m_graphicSwapchain.images[imageIndex],
                          drawImageExtent, this->m_swapchainExtent);

  utils::transitionImage(
      commandBuffer, this->m_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);
  commandBuffer.end();
}

void mvk::vkEngine::_initSynchronizationObjects() {
  vk::SemaphoreCreateInfo semaphore_info{};
  vk::FenceCreateInfo fence_info{.flags = vk::FenceCreateFlagBits::eSignaled};

  this->m_imageAvailableSempahore =
      this->m_device.createSemaphore(semaphore_info);
  this->m_renderFinishedSemaphore =
      this->m_device.createSemaphore(semaphore_info);
  this->m_inFlightFence = this->m_device.createFence(fence_info);
  this->m_immediateFence = this->m_device.createFence(fence_info);

  this->m_deletionStack.pushFunction([&]() {
    this->m_device.destroyFence(this->m_inFlightFence);
    this->m_device.destroyFence(this->m_immediateFence);
    this->m_device.destroySemaphore(this->m_imageAvailableSempahore);
    this->m_device.destroySemaphore(this->m_renderFinishedSemaphore);
  });
}

mvk::AllocatedBuffer
mvk::vkEngine::_allocateBuffer(size_t size, vk::BufferUsageFlags usage,
                               vma::AllocationCreateFlags flags,
                               vma::MemoryUsage memoryUsage) {
  vma::AllocationCreateInfo allocationInfo{
      .flags = flags,
      .usage = memoryUsage,
  };

  vk::BufferCreateInfo bufferInfo{};
  bufferInfo.setSize(size);
  bufferInfo.setUsage(usage);

  AllocatedBuffer allocBuffer;

  auto vmaAllocatedBuffer = this->m_allocator.createBuffer(
      bufferInfo, allocationInfo, &allocBuffer.allocationInfo);

  allocBuffer.buffer = vmaAllocatedBuffer.first;
  allocBuffer.allocation = vmaAllocatedBuffer.second;

  return allocBuffer;
}

void mvk::vkEngine::_destroyBuffer(const mvk::AllocatedBuffer &buffer) {
  this->m_allocator.destroyBuffer(buffer.buffer, buffer.allocation);
}

mvk::MeshData mvk::vkEngine::uploadMesh(std::span<mvk::VertexData> vertices,
                                        std::span<unsigned int> indeces) {
  const auto vertexDataSize = sizeof(mvk::VertexData) * vertices.size();
  const auto indecesDataSize = sizeof(unsigned int) * indeces.size();

  mvk::MeshData mesh;
  mesh.vertexBuffer = this->_allocateBuffer(
      vertexDataSize,
      vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eStorageBuffer |
          vk::BufferUsageFlagBits::eShaderDeviceAddress,
      vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eGpuOnly);
  mesh.indexBuffer = this->_allocateBuffer(
      indecesDataSize,
      vk::BufferUsageFlagBits::eTransferDst |
          vk::BufferUsageFlagBits::eIndexBuffer,
      vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eGpuOnly);

  vk::BufferDeviceAddressInfo addressInfo;
  addressInfo.setBuffer(mesh.vertexBuffer.buffer);
  mesh.vertexBufferAddress = this->m_device.getBufferAddress(addressInfo);

  mvk::AllocatedBuffer staginBuffer = this->_allocateBuffer(
      vertexDataSize + indecesDataSize, vk::BufferUsageFlagBits::eTransferSrc,
      vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eCpuOnly);

  void *data = staginBuffer.allocationInfo.pMappedData;

  memcpy(data, vertices.data(), vertexDataSize);
  memcpy((char *)data + vertexDataSize, indeces.data(), indecesDataSize);
  mesh.indexCount = static_cast<uint32_t>(indeces.size());

  _immediateSubmit([&](vk::CommandBuffer cmd) {
    vk::BufferCopy vertexCopy{
        .srcOffset = 0, .dstOffset = 0, .size = vertexDataSize};

    cmd.copyBuffer(staginBuffer.buffer, mesh.vertexBuffer.buffer, vertexCopy);
    vk::BufferCopy indexCopy{
        .srcOffset = vertexDataSize, .dstOffset = 0, .size = indecesDataSize};

    cmd.copyBuffer(staginBuffer.buffer, mesh.indexBuffer.buffer, indexCopy);
  });

  this->_destroyBuffer(staginBuffer);

  return mesh;
}

void mvk::vkEngine::_immediateSubmit(
    std::function<void(vk::CommandBuffer cmd)> &&function) {
  this->m_device.resetFences(1, &this->m_immediateFence);
  this->m_immediateCommandBuffer.reset();

  vk::CommandBuffer cmd = this->m_immediateCommandBuffer;

  vk::CommandBufferBeginInfo cmdBeginInfo{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
  };

  cmd.begin(cmdBeginInfo);

  function(cmd);

  cmd.end();

  vk::CommandBufferSubmitInfo commandSubmitInfo{
      .commandBuffer = cmd,
  };

  vk::SubmitInfo2 submitInfo{.commandBufferInfoCount = 1,
                             .pCommandBufferInfos = &commandSubmitInfo};
  this->m_graphicsQueue.submit2(submitInfo, this->m_immediateFence);

  this->m_device.waitForFences(this->m_immediateFence, true, 9999999);
}

void mvk::vkEngine::_initDescriptors(){
  std::array<DescriptoAllocatorGrowable::PoolSizeRatio, 1> ratios;
  ratios[0].type = vk::DescriptorType::eUniformBuffer;
  ratios[0].ratio = 1.f;
  this->m_descriptorAllocator.init(this->m_device, 1, ratios);

  mvk::DescriptorLayoutBuidler builder;
  builder.addBinding(0, vk::DescriptorType::eUniformBuffer);
  auto layout = builder.build(this->m_device, vk::ShaderStageFlagBits::eVertex);

  this->m_descriptorSet.layout = layout;
  this->m_descriptorSet.descriptor = this->m_descriptorAllocator.allocate(this->m_device, layout);

  this->m_descriptorSet.buffer = this->_allocateBuffer(sizeof(mvk::UniformDescriptorObject), vk::BufferUsageFlagBits::eUniformBuffer, vma::AllocationCreateFlagBits::eMapped | vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto);

  this->m_deletionStack.pushFunction([&](){
    std::cout << "Destroying descriptor set and associated data structures\n";
    this->m_allocator.destroyBuffer(this->m_descriptorSet.buffer.buffer, this->m_descriptorSet.buffer.allocation);
    this->m_device.destroyDescriptorSetLayout(this->m_descriptorSet.layout);
    this->m_descriptorAllocator.destroyPools(this->m_device);
  });
}

mvk::AllocatedImage mvk::vkEngine::_createImage(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
  AllocatedImage newImage;
  newImage.extent = size;
  newImage.format = format;
  
  vk::ImageCreateInfo imgInfo = utils::imageCreateInfo(format, usage, size);
  if(mipmapped)
    imgInfo.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height))));

  vma::AllocationCreateInfo allocaInfo{};
  allocaInfo.setUsage(vma::MemoryUsage::eGpuOnly);
  allocaInfo.setRequiredFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);
  
  auto imageCreationResult = this->m_allocator.createImage(imgInfo, allocaInfo);
  newImage.image = imageCreationResult.first;
  newImage.allocation = imageCreationResult.second;

  vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor;
  if(format == vk::Format::eD32Sfloat) aspectFlags = vk::ImageAspectFlagBits::eDepth;

  vk::ImageViewCreateInfo viewInfo{};

  viewInfo.subresourceRange.setLevelCount(imgInfo.mipLevels);

  newImage.imageView = this->m_device.createImageView(viewInfo);

  return newImage;
}

mvk::AllocatedImage mvk::vkEngine::_createImage(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
  size_t dataSize = size.depth * size.height * size.width*4;
  AllocatedBuffer uplodaBuffer = this->_allocateBuffer(dataSize, vk::BufferUsageFlagBits::eTransferSrc, vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eCpuToGpu);
  
  memcpy(uplodaBuffer.allocationInfo.pMappedData, data, dataSize);

  AllocatedImage newImage = this->_createImage(size, format, usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferDst, mipmapped);

  this->_immediateSubmit([&](vk::CommandBuffer cmd){
    mvk::utils::transitionImage(cmd, newImage.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    vk::BufferImageCopy copyRegion{};
    copyRegion.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
    copyRegion.imageSubresource.setLayerCount(1);
    copyRegion.setImageExtent(size);

    cmd.copyBufferToImage(uplodaBuffer.buffer, newImage.image, vk::ImageLayout::eTransferDstOptimal, copyRegion);
    
    mvk::utils::transitionImage(cmd, newImage.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
  });

  this->_destroyBuffer(uplodaBuffer);

  return newImage;
}

void mvk::vkEngine::_destroyImage(const AllocatedImage& image){
  this->m_device.destroyImageView(image.imageView);
  this->m_allocator.destroyImage(image.image, image.allocation);
}
