#ifndef VK_ENGINE_HPP
#define VK_ENGINE_HPP

#include <memory>
#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <SDL3/SDL.h>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>
#include "meshData.hpp"
#include "util_structs.hpp"
#include "DescriptorAllocator.hpp"

namespace mvk {
class vkEngine {
public:
  static vkEngine *get(SDL_Window *window);
  void init();

  void draw();

  void cleanup();

  mvk::MeshData uploadMesh(std::span<mvk::VertexData> vertices,
                           std::span<unsigned int> indeces);

private: // Methods
  vkEngine(SDL_Window *window);

  void _initVulkan();

  void _initSwapchain(uint32_t width, uint32_t height);
  void _createDrawImage();

  void _initCommandPool(uint32_t queueFamilyIndex);
  void _allocateCommandBuffer(vk::CommandBuffer &buffer);
  void immediateSubmit(std::function<void(vk::CommandBuffer cmd)> &&function);
  void _recordCommandBuffer(vk::CommandBuffer commandBuffer,
                            uint32_t imageIndex);

  void _initSynchronizationObjects();

  AllocatedBuffer _allocateBuffer(size_t size, vk::BufferUsageFlags usage,
                                  vma::AllocationCreateFlags flags,
                                  vma::MemoryUsage memoryUsage);
  void _destroyBuffer(const mvk::AllocatedBuffer &buffer);

  void _initGraphicPipeline(std::span<vk::DescriptorSetLayout> layouts);

  void _initFrameBuffers();

  void initDescriptors();
  void updateDescriptorSet(mvk::DescriptorObject &descriptor);
  void
  updateUbos(mvk::UniformDescriptorObject ubo); // TODO temporary for testing

public:
  std::vector<std::shared_ptr<MeshAsset>> testMeshes;

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

  vk::CommandPool _graphicsCommandPool;
  vk::CommandBuffer _graphicsCommandBuffer;
  vk::CommandPool _immediateCommandPool;
  vk::CommandBuffer _immediateCommandBuffer;
  vk::Fence immediateFence;

  mvk::SwapChain _graphicSwapchain;
  vk::Extent2D swapchainExtent;
  mvk::AllocatedImage _drawImage;

  // Depth testing
  mvk::AllocatedImage _depthImage;

  vk::RenderPass _renderPass;
  // temporary
  vk::PipelineLayout _graphicsPipelineLayout;
  vk::Pipeline _graphicsPipeline;

  // Synchro primitives
  vk::Semaphore _imageAvailableSempahore, _renderFinishedSemaphore;
  vk::Fence inFlightFence;

  // Buffer allocation
  vma::Allocator _allocator;

  // Descriptors
  /*vk::DescriptorPool _descriptorPool;*/
  mvk::DescriptoAllocatorGrowable descriptorAllocator;
  mvk::DescriptorObject _descriptorSet;
  mvk::UniformDescriptorObject ubo;

  // Push constants
  mvk::GPUDrawPushConstants samplePushConstants;
};
} // namespace mvk

#endif
