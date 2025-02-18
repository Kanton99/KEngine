#define VMA_IMPLEMENTATION
#include "vkEngine.hpp"
#include "utils.hpp"
#include "init.hpp"
#include "pipeline_builder.hpp"
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

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
  int width, height;
  SDL_GetWindowSize(this->_window, &width, &height);

  this->_initSwapchain(width, height);
  this->_initCommandPool(this->_graphicsQueueIndex);
  this->_allocateCommandBuffer(this->_graphicsCommandBuffer);

  this->_initGraphicPipeline();
  this->_initFrameBuffers();
  this->_initSynchronizationObjects();

  this->_initDescriptorPool(1);
  this->_allocateDescriptorSet(this->_descriptorSet);
}

void mvk::vkEngine::draw(){
  auto ret = this->_device.waitForFences(this->inFlightFence,vk::True,UINT64_MAX);
  this->_device.resetFences(this->inFlightFence);

  uint32_t imageIndex = this->_device.acquireNextImageKHR(this->graphicSwapchain.swapchain, UINT64_MAX, this->_imageAvailableSempahore).value;

  this->_graphicsCommandBuffer.reset();
  this->_recordCommandBuffer(this->_graphicsCommandBuffer, imageIndex);

  vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
  vk::Semaphore signalSemaphores[] = {this->_renderFinishedSemaphore};
  vk::SubmitInfo submitInfo{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &this->_imageAvailableSempahore,
    .pWaitDstStageMask = waitStages,
    .commandBufferCount = 1,
    .pCommandBuffers = &this->_graphicsCommandBuffer,
    .signalSemaphoreCount = 1,
    .pSignalSemaphores = signalSemaphores
  };

  this->_graphicsQueue.submit(submitInfo, this->inFlightFence);

  vk::SwapchainKHR swapchains[] = {this->graphicSwapchain.swapchain};
  vk::PresentInfoKHR prensetInfo{
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = signalSemaphores,
      .swapchainCount = 1,
      .pSwapchains = swapchains,
      .pImageIndices = &imageIndex
  };
  this->_graphicsQueue.presentKHR(prensetInfo);

}

void mvk::vkEngine::cleanup() { 
    this->_device.waitIdle();
    this->deletionStack.flush(); 
}

mvk::vkEngine::vkEngine(SDL_Window *window) : _window(window) {}

void mvk::vkEngine::_allocateCommandBuffer(vk::CommandBuffer& buffer) {
  vk::CommandBufferAllocateInfo bufferAllocationInfo{
      .commandPool = this->_commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1};
  buffer = this->_device.allocateCommandBuffers(bufferAllocationInfo).front();
}

void mvk::vkEngine::_initVulkan() {
  vkb::InstanceBuilder instanceBuilder;

  auto instRet = instanceBuilder.set_app_name("My Vulkan App")
                      .request_validation_layers(true)
                      .require_api_version(1, 3, 0)
#ifndef NDEBUG
                      .use_default_debug_messenger()
#endif // !NDEBUG
                      .build();

  this->_instance = vk::Instance(instRet.value().instance);

  this->deletionStack.pushFunction(
      [=, this]() { this->_instance.destroy(); });

#ifndef NDEBUG
  this->_debugMessanger =
      vk::DebugUtilsMessengerEXT(instRet->debug_messenger);
  this->deletionStack.pushFunction([=, this]() {
    this->_instance.destroyDebugUtilsMessengerEXT(this->_debugMessanger);
  });
#endif // !NDEBUG

  VkSurfaceKHR surface;
  if (!SDL_Vulkan_CreateSurface(this->_window,
                               static_cast<VkInstance>(this->_instance),
                               nullptr,
                               &surface)) {
    std::cerr << "Error generating surface\n";
  }
  this->_surface = vk::SurfaceKHR(surface);

  this->deletionStack.pushFunction(
      [=, this]() { this->_instance.destroySurfaceKHR(this->_surface); });

  vkb::PhysicalDeviceSelector selector{instRet.value()};
  vkb::PhysicalDevice vkbPhysDevice =
      selector.set_minimum_version(1, 1)
          .set_surface(static_cast<VkSurfaceKHR>(this->_surface))
          .select()
          .value();

  vkb::DeviceBuilder deviceBuilder{vkbPhysDevice};

  vkb::Device vkbDevice = deviceBuilder.build().value();

  this->_device = vk::Device(vkbDevice.device);
  this->_physDevice = vk::PhysicalDevice(vkbPhysDevice.physical_device);
  this->_graphicsQueue =
      vk::Queue(vkbDevice.get_queue(vkb::QueueType::graphics).value());
  this->_graphicsQueueIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

  this->deletionStack.pushFunction([=, this]() { this->_device.destroy(); });
}

void mvk::vkEngine::_initCommandPool(int queueFamilyIndex) {
  vk::CommandPoolCreateInfo poolInfo{
    .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = (uint32_t) queueFamilyIndex
  };
   
  this->_commandPool = this->_device.createCommandPool(poolInfo);

  this->deletionStack.pushFunction([&](){
    this->_device.destroyCommandPool(this->_commandPool);
  });
}

void mvk::vkEngine::_initSwapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapchainBuilder{
      static_cast<VkPhysicalDevice>(this->_physDevice),
      static_cast<VkDevice>(this->_device),
      static_cast<VkSurfaceKHR>(this->_surface)};

  vkb::Swapchain vkbSwapchain =
      swapchainBuilder
          .use_default_format_selection()
          // use vsync present mode
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
          .set_desired_extent(width, height)
          .build()
          .value();

  // store swapchain and its related images
  this->graphicSwapchain.swapchain = vkbSwapchain.swapchain;
  auto _swapchainImages = vkbSwapchain.get_images().value();
  auto _swapchainImageViews = vkbSwapchain.get_image_views().value();

  this->graphicSwapchain.images.reserve(_swapchainImages.size());
  for (auto image : _swapchainImages) {
    this->graphicSwapchain.images.emplace_back(vk::Image(image));
  }

  this->graphicSwapchain.imageViews.reserve(_swapchainImageViews.size());
  for (auto image : _swapchainImageViews) {
    this->graphicSwapchain.imageViews.emplace_back(vk::ImageView(image));
  }

  this->graphicSwapchain.format = vk::Format(vkbSwapchain.image_format);
  
  this->swapchainExtent = vkbSwapchain.extent;

  this->deletionStack.pushFunction([=, this]() {
    this->_device.destroySwapchainKHR(this->graphicSwapchain.swapchain);

    for (unsigned long i = 0; i < this->graphicSwapchain.imageViews.size();
         i++) {
      this->_device.destroyImageView(this->graphicSwapchain.imageViews[i]);
    }
  });
}

void mvk::vkEngine::_initGraphicPipeline() {
  auto vertShaderCode = readFile("../resources/shaders/compiled/vert.spv");
  auto fragShaderCode = readFile("../resources/shaders/compiled/frag.spv");

  auto vertModule = createShaderModule(vertShaderCode, this->_device);
  auto fragModule = createShaderModule(fragShaderCode, this->_device);
  
  auto pipelineLayoutInfo = vkInit::pipeline_layout_create_info(); 
  this->_trianglePipelineLayout = this->_device.createPipelineLayout(pipelineLayoutInfo);

  PipelineBuilder pipelineBuilder;

  pipelineBuilder._pipelineLayout = _trianglePipelineLayout;
  pipelineBuilder.setShaders(vertModule, fragModule);
  pipelineBuilder.setInputTopology(vk::PrimitiveTopology::eTriangleList);
  pipelineBuilder.setPolygonMode(vk::PolygonMode::eFill);
  pipelineBuilder.setCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise);
  pipelineBuilder.setMultisamplingNone();
  pipelineBuilder.disableBlending();
  pipelineBuilder.disableDepthtest();

  pipelineBuilder.setColorAttachmentFormat(this->graphicSwapchain.format);
  pipelineBuilder.setDepthFormat(vk::Format::eUndefined);

  pipelineBuilder.setRenderPass(this->_device);
  _trianglePipeline = pipelineBuilder.buildPipeline(this->_device);
  this->_renderPass = pipelineBuilder._renderPass;

  this->_device.destroyShaderModule(vertModule);
  this->_device.destroyShaderModule(fragModule);

  this->deletionStack.pushFunction([&](){
    this->_device.destroyPipelineLayout(this->_trianglePipelineLayout);
    this->_device.destroyRenderPass(this->_renderPass);
    this->_device.destroyPipeline(this->_trianglePipeline);
  });
}

void mvk::vkEngine::_initFrameBuffers(){
  this->graphicSwapchain.frameBuffers.resize(this->graphicSwapchain.imageViews.size());

  for(size_t i = 0; i < graphicSwapchain.imageViews.size(); i++){
    vk::ImageView attachments[] = {graphicSwapchain.imageViews[i]};

    vk::FramebufferCreateInfo framebufferInfo{
      .renderPass = this->_renderPass,
      .attachmentCount = 1,
      .pAttachments = attachments,
      .width = this->swapchainExtent.width,
      .height = this->swapchainExtent.height,
      .layers = 1
    };

    graphicSwapchain.frameBuffers[i] = this->_device.createFramebuffer(framebufferInfo);

  }
  this->deletionStack.pushFunction([&](){
    for(auto frameBuffer : graphicSwapchain.frameBuffers)
      this->_device.destroyFramebuffer(frameBuffer);
  });
}

void mvk::vkEngine::_initDescriptorPool(uint32_t size)
{
    vk::DescriptorPoolSize poolSize{
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = size
    };

    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    this->_descriptorPool = this->_device.createDescriptorPool(poolInfo);

    this->deletionStack.pushFunction([&]() {
        this->_device.destroyDescriptorPool(this->_descriptorPool);
        });
}

void mvk::vkEngine::_allocateDescriptorSet(mvk::DescriptorObject& descriptorSet)
{
    vk::DescriptorSetLayoutBinding descriptorBinding{
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1
    };

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayout{ //TODO make it a parameter
        .bindingCount = 1,
        .pBindings = &descriptorBinding
    };

    vk::DescriptorSetLayout layout = this->_device.createDescriptorSetLayout(descriptorSetLayout);
    descriptorSet.layout = layout;

    vk::DescriptorSetAllocateInfo allocInfo{
        .descriptorPool = this->_descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout
    };

    descriptorSet.descriptor = this->_device.allocateDescriptorSets(allocInfo)[0];

    this->deletionStack.pushFunction([&]() {
        this->_device.destroyDescriptorSetLayout(descriptorSet.layout);
        this->_device.freeDescriptorSets(this->_descriptorPool, descriptorSet.descriptor);
    });
}

void mvk::vkEngine::_recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex){
  vk::CommandBufferBeginInfo beginInfo{
    
  };
  commandBuffer.begin(beginInfo);
  vk::ClearColorValue color_value{};
  color_value.setFloat32({0.f,0.f,0.f,0.f});
  vk::ClearValue clear_color{
    .color = color_value
  };

  vk::RenderPassBeginInfo render_pass_info{
    .renderPass = this->_renderPass,
    .framebuffer = this->graphicSwapchain.frameBuffers[imageIndex],
    /*.renderArea.offset = {0,0},
    .renderArea.extent = this->swapchainExtent,*/
    .renderArea = {
          .offset = {0,0},
          .extent = this->swapchainExtent
    },
    .clearValueCount = 1,
    .pClearValues = &clear_color
  };

  commandBuffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, this->_trianglePipeline);

  vk::Viewport viewport{
    .x = 0.f,
    .y = 0.f,
    .width = static_cast<float>(this->swapchainExtent.width),
    .height = static_cast<float>(this->swapchainExtent.height),
    .minDepth = 0.f,
    .maxDepth = 1.f
  };
  commandBuffer.setViewport(0, viewport);

  vk::Rect2D scissors{
    .offset = {0, 0},
    .extent = swapchainExtent
  };

  commandBuffer.setScissor(0, scissors);

  commandBuffer.draw(3, 1, 0, 0);

  commandBuffer.endRenderPass();

  commandBuffer.end();
}

void mvk::vkEngine::_initSynchronizationObjects(){
  vk::SemaphoreCreateInfo semaphore_info{};
  vk::FenceCreateInfo fence_info{
    .flags = vk::FenceCreateFlagBits::eSignaled
  };

  this->_imageAvailableSempahore = this->_device.createSemaphore(semaphore_info);
  this->_renderFinishedSemaphore = this->_device.createSemaphore(semaphore_info);
  this->inFlightFence = this->_device.createFence(fence_info);

  this->deletionStack.pushFunction([&](){
    this->_device.destroyFence(this->inFlightFence);
    this->_device.destroySemaphore(this->_imageAvailableSempahore);
    this->_device.destroySemaphore(this->_renderFinishedSemaphore);
  });
}

mvk::AllocatedBuffer mvk::vkEngine::_allocateBuffer(size_t size, vk::BufferUsageFlagBits usage, vma::MemoryUsage memoryUsage){
  vma::AllocationCreateInfo allocationInfo{
    .flags = vma::AllocationCreateFlagBits::eMapped,
    .usage = memoryUsage,
  };

  vk::BufferCreateInfo bufferInfo{
    .size = size,
    .usage = usage
  };
  
  AllocatedBuffer allocBuffer;

  auto vmaAllocatedBuffer = this->_allocator.createBuffer(bufferInfo,allocationInfo);

  allocBuffer.buffer = vmaAllocatedBuffer.first;
  allocBuffer.allocation = vmaAllocatedBuffer.second;

  return allocBuffer;
}

void mvk::vkEngine::_destroyBuffer(const mvk::AllocatedBuffer& buffer){
  this->_allocator.destroyBuffer(buffer.buffer, buffer.allocation); 
}
