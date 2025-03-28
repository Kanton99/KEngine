#ifndef UTIL_STRUCTS_HPP
#define UTIL_STRUCTS_HPP

#include <functional>
#include <glm/glm.hpp>
#include <stack>
#include <vector>
#include <vulkan-memory-allocator-hpp/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace mvk {
struct SwapChain {
  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> images;
  std::vector<vk::ImageView> imageViews;
  std::vector<vk::Framebuffer> frameBuffers;
  vk::Format format;
};

struct DeletionStack {
  std::stack<std::function<void()>> deletors;

  void pushFunction(std::function<void()> &&function) {
    deletors.push(function);
  }

  void flush() {
    while (!deletors.empty()) {
      auto func = deletors.top();
      func();
      deletors.pop();
    }
  }
};

struct AllocatedBuffer {
  vk::Buffer buffer;
  vma::Allocation allocation;
  vma::AllocationInfo allocationInfo;
};

struct AllocatedImage {
  vk::Image image;
  vk::ImageView imageView;
  vk::Extent3D extent;
  vk::Format format;
  vma::Allocation allocation;
};

struct DescriptorObject {
  vk::DescriptorSet descriptor;
  vk::DescriptorSetLayout layout;
  void *mappedMemory;
  AllocatedBuffer buffer;
};

struct UniformDescriptorObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

//Mesh Data
struct MeshData {
  AllocatedBuffer vertexBuffer;
  vk::DeviceAddress vertexBufferAddress;
  AllocatedBuffer indexBuffer;
  unsigned int indexCount;
  DescriptorObject descriptor;
};

struct GeoSurface{
  uint32_t startIndex;
  uint32_t count;
};

struct MeshAsset {
  std::string name;

  std::vector<GeoSurface> surfaces;
  MeshData buffers;
};
} // namespace mvk

#endif
