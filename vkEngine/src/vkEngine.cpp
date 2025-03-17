#define VMA_IMPLEMENTATION
#include "vkEngine/vkEngine.hpp"
#include "vkEngine/infoCreator.hpp"
#include "vkEngine/pipeline_builder.hpp"
#include "vkEngine/utils.hpp"
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

mvk::vkEngine *mvk::vkEngine::_engine = nullptr;

mvk::vkEngine *mvk::vkEngine::get(SDL_Window *window) {
  if (!_engine) {
    _engine = new mvk::vkEngine(window);
  }
  return _engine;
}

void mvk::vkEngine::init() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif // VULKAN_HPP_DISPATCH_LOADER_DYNAMIC

  this->_initVulkan();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
  //
  int width, height;
  SDL_GetWindowSize(this->_window, &width, &height);

  vma::VulkanFunctions vulkanFunctions{
      .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = &vkGetDeviceProcAddr};
  vma::AllocatorCreateInfo allocatorInfo{
      .flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress,
      .physicalDevice = this->_physDevice,
      .device = this->_device,
      .pVulkanFunctions = &vulkanFunctions,
      .instance = this->_instance,
      .vulkanApiVersion = vk::ApiVersion13,
  };
  this->_allocator = vma::createAllocator(allocatorInfo);
  this->deletionStack.pushFunction([&]() { this->_allocator.destroy(); });

  this->_initSwapchain(static_cast<uint32_t>(width),
                       static_cast<uint32_t>(height));
  this->_initCommandPool(this->_graphicsQueueIndex);
  this->_allocateCommandBuffer(this->_graphicsCommandBuffer);
  this->_initDescriptorPool(1);
  this->_allocateDescriptorSet<UniformDescriptorObject>(this->_descriptorSet);

  this->updateDescriptorSet(this->_descriptorSet);
  this->updateUbos(this->ubo);

  std::vector<vk::DescriptorSetLayout> layouts = {this->_descriptorSet.layout};
  this->_initGraphicPipeline(layouts);
  this->_initSynchronizationObjects();
  this->deletionStack.pushFunction([&]() {
    for (const auto &buffer : testMeshes) {
      this->_destroyBuffer(buffer->buffers.vertexBuffer);
      this->_destroyBuffer(buffer->buffers.indexBuffer);
    }
  });
}

void mvk::vkEngine::draw() {
  this->_device.waitForFences(this->inFlightFence, vk::True, UINT64_MAX);
  this->_device.resetFences(this->inFlightFence);

  uint32_t imageIndex =
      this->_device
          .acquireNextImageKHR(this->_graphicSwapchain.swapchain, UINT64_MAX,
                               this->_imageAvailableSempahore)
          .value;

  this->_graphicsCommandBuffer.reset();

  this->_recordCommandBuffer(this->_graphicsCommandBuffer, imageIndex);

  auto cmdSubmitInfo =
      utils::commandBufferSubmitInfo(this->_graphicsCommandBuffer);

  auto signalInfo = utils::semaphoreSubmitInfo(
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      this->_renderFinishedSemaphore);
  auto waitInfo = utils::semaphoreSubmitInfo(
      vk::PipelineStageFlagBits2::eAllGraphics, this->_imageAvailableSempahore);

  auto submitInfo2 = utils::submitInfo(&cmdSubmitInfo, &signalInfo, &waitInfo);
  this->_graphicsQueue.submit2(submitInfo2, this->inFlightFence);

  vk::PresentInfoKHR prensetInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &this->_renderFinishedSemaphore,
      .swapchainCount = 1,
      .pSwapchains = &this->_graphicSwapchain.swapchain,
      .pImageIndices = &imageIndex};
  this->_graphicsQueue.presentKHR(prensetInfo);
}

void mvk::vkEngine::cleanup() {
  this->_device.waitIdle();
  this->deletionStack.flush();
}

mvk::vkEngine::vkEngine(SDL_Window *window) : _window(window) {}

void mvk::vkEngine::_allocateCommandBuffer(vk::CommandBuffer &buffer) {
  vk::CommandBufferAllocateInfo bufferAllocationInfo{
      .commandPool = this->_graphicsCommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1};
  buffer = this->_device.allocateCommandBuffers(bufferAllocationInfo).front();

  vk::CommandBufferAllocateInfo immediateAllocationInfo{
      .commandPool = this->_immediateCommandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1,
  };
  this->_immediateCommandBuffer =
      this->_device.allocateCommandBuffers(immediateAllocationInfo).front();
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

  this->_instance = vk::Instance(instRet.value().instance);

  this->deletionStack.pushFunction([=, this]() { this->_instance.destroy(); });

#ifndef NDEBUG
  this->_debugMessanger = vk::DebugUtilsMessengerEXT(instRet->debug_messenger);
  this->deletionStack.pushFunction([=, this]() {
    this->_instance.destroyDebugUtilsMessengerEXT(this->_debugMessanger);
  });
#endif // !NDEBUG

  VkSurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(this->_window,
                                static_cast<VkInstance>(this->_instance),
                                nullptr, &surface)) {
    std::cerr << "Error generating surface\n";
  }
  this->_surface = vk::SurfaceKHR(surface);

  this->deletionStack.pushFunction(
      [=, this]() { this->_instance.destroySurfaceKHR(this->_surface); });

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
          .set_surface(static_cast<VkSurfaceKHR>(this->_surface))
          .set_required_features_12(
              static_cast<VkPhysicalDeviceVulkan12Features>(features_12))
          .set_required_features_13(
              static_cast<VkPhysicalDeviceVulkan13Features>(features_13))
          .select()
          .value();

  vkb::DeviceBuilder deviceBuilder{vkbPhysDevice};

  vkb::Device vkbDevice = deviceBuilder.build().value();

  this->_device = vk::Device(vkbDevice.device);
  this->_physDevice = vk::PhysicalDevice(vkbPhysDevice.physical_device);
  this->_graphicsQueue =
      vk::Queue(vkbDevice.get_queue(vkb::QueueType::graphics).value());
  this->_graphicsQueueIndex =
      vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

  this->deletionStack.pushFunction([&]() {
    for (const auto &buffer : testMeshes) {
      this->_destroyBuffer(buffer->buffers.vertexBuffer);
      this->_destroyBuffer(buffer->buffers.indexBuffer);
    }
  });
  this->deletionStack.pushFunction([=, this]() { this->_device.destroy(); });
}

void mvk::vkEngine::_initCommandPool(uint32_t queueFamilyIndex) {
  vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = (uint32_t)queueFamilyIndex};

  this->_graphicsCommandPool = this->_device.createCommandPool(poolInfo);
  this->_immediateCommandPool = this->_device.createCommandPool(poolInfo);

  this->deletionStack.pushFunction([&]() {
    this->_device.destroyCommandPool(this->_graphicsCommandPool);
    this->_device.destroyCommandPool(this->_immediateCommandPool);
  });
}

void mvk::vkEngine::_initSwapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapchainBuilder{
      static_cast<VkPhysicalDevice>(this->_physDevice),
      static_cast<VkDevice>(this->_device),
      static_cast<VkSurfaceKHR>(this->_surface)};

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
  this->_graphicSwapchain.swapchain = vkbSwapchain.swapchain;
  auto _swapchainImages = vkbSwapchain.get_images().value();
  auto _swapchainImageViews = vkbSwapchain.get_image_views().value();

  this->_graphicSwapchain.images.reserve(_swapchainImages.size());
  for (auto image : _swapchainImages) {
    this->_graphicSwapchain.images.emplace_back(vk::Image(image));
  }

  this->_graphicSwapchain.imageViews.reserve(_swapchainImageViews.size());
  for (auto image : _swapchainImageViews) {
    this->_graphicSwapchain.imageViews.emplace_back(vk::ImageView(image));
  }

  this->_graphicSwapchain.format = vk::Format(vkbSwapchain.image_format);

  this->swapchainExtent = vkbSwapchain.extent;

  // Creating Depth testing image
  this->_depthImage.format = vk::Format::eD32Sfloat;
  this->_depthImage.extent.width = width;
  this->_depthImage.extent.height = height;
  this->_depthImage.extent.depth = 1;

  vk::ImageCreateInfo imageInfo = mvk::utils::imageCreateInfo(
      this->_depthImage.format, vk::ImageUsageFlagBits::eDepthStencilAttachment,
      this->_depthImage.extent);

  vma::AllocationCreateInfo allocCreateInfo{
      .usage = vma::MemoryUsage::eAuto,
  };

  auto allocatedImage =
      this->_allocator.createImage(imageInfo, allocCreateInfo);
  this->_depthImage.image = allocatedImage.first;
  this->_depthImage.allocation = allocatedImage.second;

  vk::ImageViewCreateInfo viewInfo = mvk::utils::imageViewCreateInfo(
      this->_depthImage.format, this->_depthImage.image,
      vk::ImageAspectFlagBits::eDepth);

  this->_depthImage.imageView = this->_device.createImageView(viewInfo);

  this->_createDrawImage();

  this->deletionStack.pushFunction([=, this]() {
    this->_device.destroySwapchainKHR(this->_graphicSwapchain.swapchain);

    for (unsigned long i = 0; i < this->_graphicSwapchain.imageViews.size();
         i++) {
      this->_device.destroyImageView(this->_graphicSwapchain.imageViews[i]);
    }

    this->_device.destroyImageView(this->_depthImage.imageView);
    this->_allocator.destroyImage(this->_depthImage.image,
                                  this->_depthImage.allocation);
  });
}

void mvk::vkEngine::_createDrawImage() {
  _drawImage.extent = {
      this->swapchainExtent.width,
      this->swapchainExtent.height,
      1,
  };

  _drawImage.format = vk::Format::eR16G16B16A16Sfloat;

  vk::ImageUsageFlags drawImageFlags = vk::ImageUsageFlagBits::eTransferSrc |
                                       vk::ImageUsageFlagBits::eTransferSrc |
                                       vk::ImageUsageFlagBits::eStorage |
                                       vk::ImageUsageFlagBits::eColorAttachment;

  vk::ImageCreateInfo drawImageInfo = mvk::utils::imageCreateInfo(
      this->_drawImage.format, drawImageFlags, _drawImage.extent);

  vma::AllocationCreateInfo imageAllocInfo{
      .usage = vma::MemoryUsage::eGpuOnly,
      .requiredFlags = vk::MemoryPropertyFlagBits::eDeviceLocal};

  auto imageAllocation =
      this->_allocator.createImage(drawImageInfo, imageAllocInfo);
  _drawImage.image = imageAllocation.first;
  _drawImage.allocation = imageAllocation.second;

  auto imageViewInfo = mvk::utils::imageViewCreateInfo(
      _drawImage.format, _drawImage.image, vk::ImageAspectFlagBits::eColor);
  this->_drawImage.imageView = this->_device.createImageView(imageViewInfo);

  this->deletionStack.pushFunction([&]() {
    this->_device.destroyImageView(this->_drawImage.imageView);
    this->_allocator.destroyImage(this->_drawImage.image,
                                  this->_drawImage.allocation);
  });
}

void mvk::vkEngine::_initGraphicPipeline(
    std::span<vk::DescriptorSetLayout> layouts) {
  auto vertShaderCode =
      utils::readFile("resources/shaders/compiled/base_vertex.vert.spv");
  auto fragShaderCode =
      utils::readFile("resources/shaders/compiled/base_frag.frag.spv");

  auto vertModule = utils::createShaderModule(vertShaderCode, this->_device);
  auto fragModule = utils::createShaderModule(fragShaderCode, this->_device);

  vk::PushConstantRange pushConstantRanges{};
  pushConstantRanges.setOffset(0);
  pushConstantRanges.setSize(sizeof(GPUDrawPushConstants));
  pushConstantRanges.setStageFlags(vk::ShaderStageFlagBits::eVertex);

  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.setSetLayouts(layouts);
  pipelineLayoutCreateInfo.setPushConstantRanges(pushConstantRanges);
  this->_graphicsPipelineLayout =
      this->_device.createPipelineLayout(pipelineLayoutCreateInfo);

  PipelineBuilder pipelineBuilder;

  pipelineBuilder._pipelineLayout = _graphicsPipelineLayout;
  pipelineBuilder.setShaders(vertModule, fragModule);
  pipelineBuilder.setInputTopology(vk::PrimitiveTopology::eTriangleList);
  pipelineBuilder.setPolygonMode(vk::PolygonMode::eFill);
  pipelineBuilder.setCullMode(vk::CullModeFlagBits::eNone,
                              vk::FrontFace::eClockwise);
  pipelineBuilder.setMultisamplingNone();
  pipelineBuilder.disableBlending();
  pipelineBuilder.enableDepthtest(true, vk::CompareOp::eGreaterOrEqual);
  pipelineBuilder.setColorAttachmentFormat(this->_drawImage.format);
  pipelineBuilder.setDepthFormat(_depthImage.format);

  _graphicsPipeline = pipelineBuilder.buildPipeline(this->_device);

  this->_device.destroyShaderModule(vertModule);
  this->_device.destroyShaderModule(fragModule);

  this->deletionStack.pushFunction([&]() {
    this->_device.destroyPipelineLayout(this->_graphicsPipelineLayout);
    // this->_device.destroyRenderPass(this->_renderPass);
    this->_device.destroyPipeline(this->_graphicsPipeline);
  });
}

void mvk::vkEngine::_initFrameBuffers() {
  this->_graphicSwapchain.frameBuffers.resize(
      this->_graphicSwapchain.imageViews.size());

  for (size_t i = 0; i < _graphicSwapchain.imageViews.size(); i++) {
    vk::ImageView attachments[] = {_graphicSwapchain.imageViews[i]};

    vk::FramebufferCreateInfo framebufferInfo{
        .renderPass = this->_renderPass,
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = this->swapchainExtent.width,
        .height = this->swapchainExtent.height,
        .layers = 1};

    _graphicSwapchain.frameBuffers[i] =
        this->_device.createFramebuffer(framebufferInfo);
  }
  this->deletionStack.pushFunction([&]() {
    for (auto frameBuffer : _graphicSwapchain.frameBuffers)
      this->_device.destroyFramebuffer(frameBuffer);
  });
}

void mvk::vkEngine::_initDescriptorPool(uint32_t size) {
  vk::DescriptorPoolSize poolSize{.type = vk::DescriptorType::eUniformBuffer,
                                  .descriptorCount = size};

  vk::DescriptorPoolCreateInfo poolInfo{
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize,
  };

  this->_descriptorPool = this->_device.createDescriptorPool(poolInfo);

  this->deletionStack.pushFunction(
      [&]() { this->_device.destroyDescriptorPool(this->_descriptorPool); });
}

void mvk::vkEngine::updateDescriptorSet(mvk::DescriptorObject &descriptor) {
  vk::DescriptorBufferInfo bufferInfo{
      .buffer = descriptor.buffer.buffer,
      .offset = descriptor.buffer.allocationInfo.offset,
      .range = vk::WholeSize};
  vk::WriteDescriptorSet writeDS{.dstSet = descriptor.descriptor,
                                 .dstBinding = 0,
                                 .descriptorCount = 1,
                                 .descriptorType =
                                     vk::DescriptorType::eUniformBuffer,
                                 .pBufferInfo = &bufferInfo};

  this->_device.updateDescriptorSets(writeDS, nullptr);
}

void mvk::vkEngine::updateUbos(mvk::UniformDescriptorObject ubo) {
  static float rotationAngle;
  ubo.model = glm::identity<glm::mat4>();
  ubo.model =
      glm::rotate(ubo.model, glm::radians(rotationAngle++), glm::vec3(0, 1, 0));

  ubo.proj = glm::perspectiveRH_ZO(glm::radians(45.f),
                                   (float)this->_drawImage.extent.width /
                                       (float)this->_drawImage.extent.height,
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

  memcpy(this->_descriptorSet.buffer.allocationInfo.pMappedData, &ubo,
         sizeof(ubo));
}

template <typename T>
void mvk::vkEngine::_allocateDescriptorSet(
    mvk::DescriptorObject &descriptorSet) {
  descriptorSet.buffer = this->_allocateBuffer(
      sizeof(T), vk::BufferUsageFlagBits::eUniformBuffer,
      vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
          vma::AllocationCreateFlagBits::eMapped,
      vma::MemoryUsage::eAuto);
  this->deletionStack.pushFunction(
      [&]() { this->_destroyBuffer(descriptorSet.buffer); });
  vk::DescriptorSetLayoutBinding descriptorBinding{
      .descriptorType = vk::DescriptorType::eUniformBuffer,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eVertex};

  vk::DescriptorSetLayoutCreateInfo descriptorSetLayout{
      // TODO make it a parameter
      .bindingCount = 1,
      .pBindings = &descriptorBinding};

  vk::DescriptorSetLayout layout =
      this->_device.createDescriptorSetLayout(descriptorSetLayout);
  descriptorSet.layout = layout;

  vk::DescriptorSetAllocateInfo allocInfo{.descriptorPool =
                                              this->_descriptorPool,
                                          .descriptorSetCount = 1,
                                          .pSetLayouts = &layout};

  descriptorSet.descriptor = this->_device.allocateDescriptorSets(allocInfo)[0];

  this->deletionStack.pushFunction([&]() {
    this->_device.destroyDescriptorSetLayout(descriptorSet.layout);
    this->_device.freeDescriptorSets(this->_descriptorPool,
                                     descriptorSet.descriptor);
  });
}

void mvk::vkEngine::_recordCommandBuffer(vk::CommandBuffer commandBuffer,
                                         uint32_t imageIndex) {
  vk::CommandBufferBeginInfo beginInfo{};
  commandBuffer.begin(beginInfo);

  utils::transitionImage(commandBuffer, this->_depthImage.image,
                         vk::ImageLayout::eUndefined,
                         vk::ImageLayout::eDepthAttachmentOptimal);
  utils::transitionImage(commandBuffer, this->_drawImage.image,
                         vk::ImageLayout::eUndefined,
                         vk::ImageLayout::eColorAttachmentOptimal);
  utils::transitionImage(
      commandBuffer, this->_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
  vk::RenderingAttachmentInfo depthAttachment{
      .imageView = _depthImage.imageView,
      .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.depthStencil = {.depth = 0.f}}};

  std::array<float, 4> clearColor = {0.f};
  vk::RenderingAttachmentInfo colorAttachment{
      .imageView = this->_drawImage.imageView,
      .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .clearValue = {.color = {clearColor}}};

  vk::RenderingInfo renderInfo{
      .renderArea = {.extent = {.width = this->_drawImage.extent.width,
                                .height = this->_drawImage.extent.height}},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachment,
      .pDepthAttachment = &depthAttachment,
  };

  commandBuffer.beginRendering(renderInfo);

  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                             this->_graphicsPipeline);

  vk::Viewport viewport{.x = 0.f,
                        .y = 0.f,
                        .width = (float)this->_drawImage.extent.width,
                        .height = (float)this->_drawImage.extent.height,
                        .minDepth = 0.f,
                        .maxDepth = 1.f};
  commandBuffer.setViewport(0, viewport);

  vk::Rect2D scissors{.offset = {0, 0}, .extent = this->swapchainExtent};

  commandBuffer.setScissor(0, scissors);

  this->updateDescriptorSet(this->_descriptorSet);
  this->updateUbos(this->ubo);

  vk::DeviceSize offsets[] = {0};
  /*commandBuffer.bindVertexBuffers(0, this->tmpMesh.vertexBuffer.buffer,*/
  /*                                offsets);*/

  commandBuffer.bindIndexBuffer(this->testMeshes[2]->buffers.indexBuffer.buffer,
                                offsets[0], vk::IndexType::eUint32);

  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   this->_graphicsPipelineLayout, 0,
                                   this->_descriptorSet.descriptor, nullptr);
  /*commandBuffer.draw(3, 1, 0, 0);*/

  this->samplePushConstants.vertexBuffer =
      this->testMeshes[2]->buffers.vertexBufferAddress;
  commandBuffer.pushConstants<GPUDrawPushConstants>(
      this->_graphicsPipelineLayout, vk::ShaderStageFlagBits::eVertex, 0,
      this->samplePushConstants);
  commandBuffer.drawIndexed(this->testMeshes[2]->buffers.indexCount, 1, 0, 0,
                            0);

  commandBuffer.endRendering();

  mvk::utils::transitionImage(this->_graphicsCommandBuffer,
                              this->_drawImage.image,
                              vk::ImageLayout::eColorAttachmentOptimal,
                              vk::ImageLayout::eTransferSrcOptimal);
  mvk::utils::transitionImage(
      this->_graphicsCommandBuffer, this->_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferDstOptimal);

  vk::Extent2D drawImageExtent{this->_drawImage.extent.width,
                               this->_drawImage.extent.height};
  utils::copyImageToImage(commandBuffer, this->_drawImage.image,
                          this->_graphicSwapchain.images[imageIndex],
                          drawImageExtent, this->swapchainExtent);

  utils::transitionImage(
      commandBuffer, this->_graphicSwapchain.images[imageIndex],
      vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);
  commandBuffer.end();
}

void mvk::vkEngine::_initSynchronizationObjects() {
  vk::SemaphoreCreateInfo semaphore_info{};
  vk::FenceCreateInfo fence_info{.flags = vk::FenceCreateFlagBits::eSignaled};

  this->_imageAvailableSempahore =
      this->_device.createSemaphore(semaphore_info);
  this->_renderFinishedSemaphore =
      this->_device.createSemaphore(semaphore_info);
  this->inFlightFence = this->_device.createFence(fence_info);
  this->immediateFence = this->_device.createFence(fence_info);

  this->deletionStack.pushFunction([&]() {
    this->_device.destroyFence(this->inFlightFence);
    this->_device.destroyFence(this->immediateFence);
    this->_device.destroySemaphore(this->_imageAvailableSempahore);
    this->_device.destroySemaphore(this->_renderFinishedSemaphore);
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

  auto vmaAllocatedBuffer = this->_allocator.createBuffer(
      bufferInfo, allocationInfo, &allocBuffer.allocationInfo);

  allocBuffer.buffer = vmaAllocatedBuffer.first;
  allocBuffer.allocation = vmaAllocatedBuffer.second;

  return allocBuffer;
}

void mvk::vkEngine::_destroyBuffer(const mvk::AllocatedBuffer &buffer) {
  this->_allocator.destroyBuffer(buffer.buffer, buffer.allocation);
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
  mesh.vertexBufferAddress = this->_device.getBufferAddress(addressInfo);

  mvk::AllocatedBuffer staginBuffer = this->_allocateBuffer(
      vertexDataSize + indecesDataSize, vk::BufferUsageFlagBits::eTransferSrc,
      vma::AllocationCreateFlagBits::eMapped, vma::MemoryUsage::eCpuOnly);

  void *data = staginBuffer.allocationInfo.pMappedData;

  memcpy(data, vertices.data(), vertexDataSize);
  memcpy((char *)data + vertexDataSize, indeces.data(), indecesDataSize);
  mesh.indexCount = static_cast<uint32_t>(indeces.size());

  immediateSubmit([&](vk::CommandBuffer cmd) {
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

void mvk::vkEngine::immediateSubmit(
    std::function<void(vk::CommandBuffer cmd)> &&function) {
  this->_device.resetFences(1, &this->immediateFence);
  this->_immediateCommandBuffer.reset();

  vk::CommandBuffer cmd = this->_immediateCommandBuffer;

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
  this->_graphicsQueue.submit2(submitInfo, this->immediateFence);

  this->_device.waitForFences(this->immediateFence, true, 9999999);
}
