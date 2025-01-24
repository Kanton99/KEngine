#include "vkEngine.h"
#include "utils.hpp"
#include "init.hpp"
#include "pipeline_builder.hpp"
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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

  this->_init_vulkan();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
  int width, height;
  SDL_GetWindowSize(this->_window, &width, &height);

  this->_init_swapchain(width, height);

  this->_init_graphic_pipeline();
  this->_init_frame_buffers();
}

void mvk::vkEngine::cleanup() { this->deletion_stack.flush(); }

mvk::vkEngine::vkEngine(SDL_Window *window) : _window(window) {}

void mvk::vkEngine::_allocate_command_buffer(vk::CommandBuffer buffer) {
  vk::CommandBufferAllocateInfo buffer_allocation_info{
      .commandPool = this->_command_pool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1};
  buffer = this->_device.allocateCommandBuffers(buffer_allocation_info).front();
}

void mvk::vkEngine::_record_command_buffer() {}

void mvk::vkEngine::_init_vulkan() {
  vkb::InstanceBuilder instance_builder;

  auto inst_ret = instance_builder.set_app_name("My Vulkan App")
                      .request_validation_layers(true)
                      .require_api_version(1, 3, 0)
#ifndef NDEBUG
                      .use_default_debug_messenger()
#endif // !NDEBUG
                      .build();

  this->_instance = vk::Instance(inst_ret.value().instance);

  this->deletion_stack.push_function(
      [=, this]() { this->_instance.destroy(); });

#ifndef NDEBUG
  this->_debug_messanger =
      vk::DebugUtilsMessengerEXT(inst_ret->debug_messenger);
  this->deletion_stack.push_function([=, this]() {
    this->_instance.destroyDebugUtilsMessengerEXT(this->_debug_messanger);
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
  this->deletion_stack.push_function(
      [=, this]() { this->_instance.destroySurfaceKHR(this->_surface); });

  vkb::PhysicalDeviceSelector selector{inst_ret.value()};
  vkb::PhysicalDevice vkb_phys_device =
      selector.set_minimum_version(1, 1)
          .set_surface(static_cast<VkSurfaceKHR>(this->_surface))
          .select()
          .value();

  vkb::DeviceBuilder device_builder{vkb_phys_device};

  vkb::Device vkb_device = device_builder.build().value();

  this->_device = vk::Device(vkb_device.device);
  this->_phys_device = vk::PhysicalDevice(vkb_phys_device.physical_device);
  this->_graphics_queue =
      vk::Queue(vkb_device.get_queue(vkb::QueueType::graphics).value());

  this->deletion_stack.push_function([=, this]() { this->_device.destroy(); });
}

void mvk::vkEngine::_init_command_pool(int queue_family_index) {}

void mvk::vkEngine::_init_swapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapchainBuilder{
      static_cast<VkPhysicalDevice>(this->_phys_device),
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
  this->graphic_swapchain.swapchain = vkbSwapchain.swapchain;
  auto _swapchainImages = vkbSwapchain.get_images().value();
  auto _swapchainImageViews = vkbSwapchain.get_image_views().value();

  this->graphic_swapchain.images.reserve(_swapchainImages.size());
  for (auto image : _swapchainImages) {
    this->graphic_swapchain.images.emplace_back(vk::Image(image));
  }

  this->graphic_swapchain.imageViews.reserve(_swapchainImageViews.size());
  for (auto image : _swapchainImageViews) {
    this->graphic_swapchain.imageViews.emplace_back(vk::ImageView(image));
  }

  this->graphic_swapchain.format = vk::Format(vkbSwapchain.image_format);
  
  this->swapchain_extent = vkbSwapchain.extent;

  this->deletion_stack.push_function([=, this]() {
    this->_device.destroySwapchainKHR(this->graphic_swapchain.swapchain);

    for (unsigned long i = 0; i < this->graphic_swapchain.imageViews.size();
         i++) {
      this->_device.destroyImageView(this->graphic_swapchain.imageViews[i]);
    }
  });
}

void mvk::vkEngine::_init_graphic_pipeline() {
  auto vert_shader_code = read_file("../resources/shaders/compiled/vert.spv");
  auto frag_shader_code = read_file("../resources/shaders/compiled/frag.spv");

  auto vert_module = create_shader_module(vert_shader_code, this->_device);
  auto frag_module = create_shader_module(frag_shader_code, this->_device);
  
  auto pipeline_layout_info = vk_init::pipeline_layout_create_info(); 
  this->_triangle_pipeline_layout = this->_device.createPipelineLayout(pipeline_layout_info);

  Pipeline_builder pipeline_builder;

  pipeline_builder._pipeline_layout = _triangle_pipeline_layout;
  pipeline_builder.set_shaders(vert_module, frag_module);
  pipeline_builder.set_input_topology(vk::PrimitiveTopology::eTriangleList);
  pipeline_builder.set_polygon_mode(vk::PolygonMode::eFill);
  pipeline_builder.set_cull_mode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise);
  pipeline_builder.set_multisampling_none();
  pipeline_builder.disable_blending();
  pipeline_builder.disable_depthtest();

  pipeline_builder.set_color_attachment_format(this->graphic_swapchain.format);
  pipeline_builder.set_depth_format(vk::Format::eUndefined);

  pipeline_builder.set_render_pass(this->_device);
  _triangle_pipeline = pipeline_builder.build_pipeline(this->_device);
  this->_render_pass = pipeline_builder._render_pass;

  this->_device.destroyShaderModule(vert_module);
  this->_device.destroyShaderModule(frag_module);

  this->deletion_stack.push_function([&](){
    this->_device.destroyPipelineLayout(this->_triangle_pipeline_layout);
    this->_device.destroyRenderPass(this->_render_pass);
    this->_device.destroyPipeline(this->_triangle_pipeline);
  });
}

void mvk::vkEngine::_init_frame_buffers(){
  this->graphic_swapchain.frameBuffers.resize(this->graphic_swapchain.imageViews.size());

  for(size_t i = 0; i < graphic_swapchain.imageViews.size(); i++){
    vk::ImageView attachments[] = {graphic_swapchain.imageViews[i]};

    vk::FramebufferCreateInfo framebuffer_info{
      .renderPass = this->_render_pass,
      .attachmentCount = 1,
      .pAttachments = attachments,
      .width = this->swapchain_extent.width,
      .height = this->swapchain_extent.height,
      .layers = 1
    };

    graphic_swapchain.frameBuffers[i] = this->_device.createFramebuffer(framebuffer_info);

  }
  this->deletion_stack.push_function([&](){
    for(auto frame_buffer : graphic_swapchain.frameBuffers)
      this->_device.destroyFramebuffer(frame_buffer);
  });
}
