#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#include "util_structs.hpp"
#include <vulkan/vulkan.hpp>
#include <SDL3/SDL.h>

namespace mvk {
class vkEngine {
public:
  static vkEngine *get(SDL_Window *window);
  void init();

  void cleanup();

private: // Methods
  vkEngine(SDL_Window *window);

  void _init_vulkan();

#ifndef NDEBUG
  void _init_debug_utils();
#endif // NDEBUG

  void _init_swapchain(uint32_t width, uint32_t height);
  void _init_command_pool(int queue_family_index);
  void _allocate_command_buffer(vk::CommandBuffer buffer);
  void _record_command_buffer();

  void _init_graphic_pipeline();
  vk::ShaderModule _create_shader_module();

private: // Members
  static vkEngine *_engine;
  mvk::DeletionStack deletion_stack;

  vk::Instance _instance;
  SDL_Window *_window;
  vk::SurfaceKHR _surface;
#ifndef NDEBUG
  vk::DebugUtilsMessengerEXT _debug_messanger;
#endif // !DEBUG

  vk::Device _device;
  vk::PhysicalDevice _phys_device;

  vk::Queue _graphics_queue;

  vk::CommandPool _command_pool;

  vk::CommandBuffer _graphics_command_buffer;

  mvk::SwapChain graphic_swapchain;
  vk::Extent2D swapchain_extent;

  //temporary
  vk::PipelineLayout _triangle_pipeline_layout;
  vk::Pipeline _triangle_pipeline;
};
} // namespace mvk
