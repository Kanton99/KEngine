#ifndef VK_ENGINE_HPP
#define VK_ENGINE_HPP

#include <memory>
#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include "DescriptorAllocator.hpp"
#include "meshData.hpp"
#include "util_structs.hpp"
#include <SDL3/SDL.h>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace mvk {
class vkEngine {
public:
  static vkEngine &get(SDL_Window *window);
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
  void _immediateSubmit(std::function<void(vk::CommandBuffer cmd)> &&function);
  void _recordCommandBuffer(vk::CommandBuffer commandBuffer,
                            uint32_t imageIndex);

  void _initSynchronizationObjects();

  AllocatedBuffer _allocateBuffer(size_t size, vk::BufferUsageFlags usage,
                                  vma::AllocationCreateFlags flags,
                                  vma::MemoryUsage memoryUsage);
  void _destroyBuffer(const mvk::AllocatedBuffer &buffer);

  void _initGraphicPipeline(std::span<vk::DescriptorSetLayout> layouts);

  void _initFrameBuffers();

  void _initDescriptors();
  void _updateDescriptorSet(mvk::DescriptorObject &descriptor);
  void
  _updateUbos(mvk::UniformDescriptorObject ubo); // TODO temporary for testing

  AllocatedImage _createImage(vk::Extent3D size, vk::Format format,
                              vk::ImageUsageFlags usage,
                              bool mipmapped = false);
  AllocatedImage _createImage(void *data, vk::Extent3D size, vk::Format format,
                              vk::ImageUsageFlags usage,
                              bool mipmapped = false);
  void _destroyImage(const AllocatedImage &image);

  void _initDefaultData();

public:
  std::vector<std::shared_ptr<MeshAsset>> testMeshes;

private: // Members
  static vkEngine *m_engine;
  mvk::DeletionStack m_deletionStack;

  vk::Instance m_instance;
  SDL_Window *m_window;
  vk::SurfaceKHR m_surface;
#ifndef NDEBUG
  vk::DebugUtilsMessengerEXT m_debugMessanger;
#endif // !DEBUG

  vk::Device m_device;
  vk::PhysicalDevice m_physDevice;

  vk::Queue m_graphicsQueue;
  uint32_t m_graphicsQueueIndex;

  vk::CommandPool m_graphicsCommandPool;
  vk::CommandBuffer m_graphicsCommandBuffer;
  vk::CommandPool m_immediateCommandPool;
  vk::CommandBuffer m_immediateCommandBuffer;
  vk::Fence m_immediateFence;

  mvk::SwapChain m_graphicSwapchain;
  vk::Extent2D m_swapchainExtent;
  mvk::AllocatedImage m_drawImage;

  // Depth testing
  mvk::AllocatedImage m_depthImage;

  vk::RenderPass m_renderPass;
  // temporary
  vk::PipelineLayout m_graphicsPipelineLayout;
  vk::Pipeline m_graphicsPipeline;

  // Synchro primitives
  vk::Semaphore m_imageAvailableSempahore, m_renderFinishedSemaphore;
  vk::Fence m_inFlightFence;

  // Buffer allocation
  vma::Allocator m_allocator;

  // Descriptors
  /*vk::DescriptorPool _descriptorPool;*/
  mvk::DescriptoAllocatorGrowable m_descriptorAllocator;
  mvk::DescriptorObject m_descriptorSet;
  mvk::UniformDescriptorObject m_ubo;

  // Push constants
  mvk::GPUDrawPushConstants m_samplePushConstants;

  mvk::AllocatedImage m_whiteImage;
  mvk::AllocatedImage m_blackImage;
  mvk::AllocatedImage m_greyImage;
  mvk::AllocatedImage m_errorCheckerboardImage;
  vk::Sampler m_defaultSamplerLinear;
  vk::Sampler m_defaultSamplerNearest;

  vk::DescriptorSetLayout _singleImageDescriptorLayout;
};
} // namespace mvk

#endif
