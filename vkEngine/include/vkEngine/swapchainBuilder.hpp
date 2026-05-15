#ifndef SWAPCHAIN_BUILDER_HPP
#define SWAPCHAIN_BUILDER_HPP
#include "SDL3/SDL_video.h"
#include <vector>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace vkEngine {
class SwapchainBuilder {
  private:
	vk::SurfaceCapabilitiesKHR _surfaceCapabilities;
	vk::SurfaceFormatKHR _format;
	vk::PresentModeKHR _presentMode;
	unsigned int _minImageCount;
	vk::Extent2D _extent;

  public:
	SwapchainBuilder(vk::SurfaceCapabilitiesKHR &capabilities);
	SwapchainBuilder &chooseSwapExtent(SDL_Window &window);
	SwapchainBuilder &chooseSwapMinImageCount();
	SwapchainBuilder &chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
	SwapchainBuilder &choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
	vk::SwapchainKHR buildSwapchain(vk::SurfaceKHR &surface, const vk::Device &device);
};
} // namespace vkEngine

#endif // !SWAPCHAIN_BUILDER_HPP
