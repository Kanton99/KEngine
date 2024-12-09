#include "vkEngine.h"
#include "utils.hpp"
#include <SDL3/SDL_vulkan.h>
#include <VkBootstrap.h>
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

  this->deletion_stack.push_function([=, this]() {
    this->_device.destroySwapchainKHR(this->graphic_swapchain.swapchain);

    for (unsigned long i = 0; i < this->graphic_swapchain.imageViews.size();
         i++) {
      this->_device.destroyImageView(this->graphic_swapchain.imageViews[i]);
    }
  });
}

void mvk::vkEngine::_init_graphic_pipeline() {
  auto vertShaderCode = read_file("resources/shaders/compiled/vert.spv");
  auto fragShaderCode = read_file("resources/shaders/compiled/frag.spv");
}
