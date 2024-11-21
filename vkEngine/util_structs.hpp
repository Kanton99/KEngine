#include <functional>
#include <stack>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace mvk {
struct SwapChain {
  vk::SwapchainKHR swapchain;
  std::vector<vk::Image> images;
  std::vector<vk::ImageView> imageViews;
  vk::Format format;
};

struct DeletionStack {
  std::stack<std::function<void()>> deletors;

  void push_function(std::function<void()> &&function) {
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

} // namespace mvk
