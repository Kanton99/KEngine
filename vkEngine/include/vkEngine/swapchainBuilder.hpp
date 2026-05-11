#ifndef SWAPCHAIN_BUILDER_HPP
#define SWAPCHAIN_BUILDER_HPP
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace vkEngine {
class SwapchainBuilder {
	vk::SurfaceFormatKHR _format;
	vk::PresentModeKHR _presentMode;

  public:
	void chooseSwapExtent();
	void chooseSwapMinImageCount();
	void chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
	void choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
	vk::SwapchainKHR buildSwapchain();
};
} // namespace vkEngine

#endif // !SWAPCHAIN_BUILDER_HPP
