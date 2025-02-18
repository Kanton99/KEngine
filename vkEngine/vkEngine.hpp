#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
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

  void _initSwapchain(uint32_t width, uint32_t height);
  void _initCommandPool(int queueFamilyIndex);
  void _recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
  void _initSynchronizationObjects();

  void _allocateCommandBuffer(vk::CommandBuffer& buffer);
  AllocatedBuffer _allocateBuffer(size_t size, vk::BufferUsageFlagBits usage, vma::MemoryUsage memoryUsage);
  void _destroyBuffer(const mvk::AllocatedBuffer& buffer);

  void _initGraphicPipeline();

  void _initFrameBuffers();

  void _initDescriptorPool(uint32_t size);
  void _allocateDescriptorSet(mvk::DescriptorObject& descriptorSet);

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
  uint32_t _graphicsQueueIndex;

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

  //Buffer allocation
  vma::Allocator _allocator;

  //Descriptors
  vk::DescriptorPool _descriptorPool;
  mvk::DescriptorObject _descriptorSet;
};
} // namespace mvk
