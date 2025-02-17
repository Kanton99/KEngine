#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VMA_IMPLEMENTATION
#include "util_structs.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>
#include <SDL3/SDL.h>

namespace mvk {
class vkEngine {
public:
  static vkEngine *get(SDL_Window *window);
  void init();

  void draw();

  void cleanup();

private: // Methods
  vkEngine(SDL_Window *window);

  void _initVulkan();

#ifndef NDEBUG
  void _initDebugUtils();
#endif // NDEBUG

  void _initSwapchain(uint32_t width, uint32_t height);
  void _initPommandPool(int queue_family_index);
  void _recordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index);
  void _initSynchronizationObjects();

  void _allocateCommandBuffer(vk::CommandBuffer buffer);
  AllocatedBuffer _allocateBuffer(size_t size, vk::BufferUsageFlagBits usage, vma::MemoryUsage memory_usage);
  void _destroyBuffer(const vk::Buffer buffer);

  void _initGraphicPipeline();
  vk::ShaderModule _createShaderModule();

  void _initFrameBuffers();

private: // Members
  static vkEngine *_engine;
  mvk::DeletionStack deletionStack;

  vk::Instance _instance;
  SDL_Window *_window;
  vk::SurfaceKHR _surface;
#ifndef NDEBUG
  vk::DebugUtilsMessengerEXT _debugMessanger;
#endif // !DEBUG

  vk::Device _device;
  vk::PhysicalDevice _physDevice;

  vk::Queue _graphicsQueue;

  vk::CommandPool _commandPool;

  vk::CommandBuffer _graphicsCommandBuffer;

  mvk::SwapChain graphicSwapchain;
  vk::Extent2D swapchainExtent;

  vk::RenderPass _renderPass;
  //temporary
  vk::PipelineLayout _trianglePipelineLayout;
  vk::Pipeline _trianglePipeline;

  //Synchro primitives
  vk::Semaphore _imageAvailableSempahore, _renderFinishedSemaphore;
  vk::Fence inFlightFence;
};
} // namespace mvk
