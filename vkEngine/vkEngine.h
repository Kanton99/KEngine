#pragma once
#include <cstdint>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include "util_structs.hpp"
#include <SDL.h>
#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_extension_inspection.hpp>

namespace mvk {
class vkEngine {
public:
  static vkEngine *get(SDL_Window *window);
  void init();

  void cleanup();

private:
  vkEngine(SDL_Window *window);

  void _init_instance();
  void _init_devices();

#ifndef NDEBUG
  void _init_debug_utils();
#endif // NDEBUG

  void _init_swapchain(uint32_t width, uint32_t height);
  void _init_command_pool(int queue_family_index);
  void _allocate_command_buffer(vk::CommandBuffer buffer);
  void _record_command_buffer();

private:
  static vkEngine *_engine;
  mvk::DeletionStack stack;

  vk::Instance _instance;
  SDL_Window *_window;
  vk::SurfaceKHR _surface;
#ifndef NDEBUG
  vk::DebugUtilsMessengerEXT _debug_messanger;
#endif // !DEBUG

  vk::Device _device;
  vk::PhysicalDevice _phys_device;

  unsigned int graphics_queue_index;
  unsigned int present_queue_index;
  vk::Queue _graphics_queue;

  vk::CommandPool _command_pool;

  vk::CommandBuffer _graphics_command_buffer;

  mvk::SwapChain graphic_swapchain;
  vk::Extent2D swapchain_extent;
};
} // namespace mvk
