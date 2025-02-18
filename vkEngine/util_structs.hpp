#include <functional>
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

struct AllocatedBuffer{
  vk::Buffer buffer;
  vma::Allocation allocation;
  vma::AllocationInfo allocationInfo;
};

struct DescriptorObject {
    vk::DescriptorSet descriptor;
    vk::DescriptorSetLayout layout;
};
} // namespace mvk
