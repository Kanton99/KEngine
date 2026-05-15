#include <algorithm>
#include <vkEngine/swapchainBuilder.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace vkEngine {

SwapchainBuilder::SwapchainBuilder(vk::SurfaceCapabilitiesKHR &capabilities) :
	_surfaceCapabilities(capabilities) {}

SwapchainBuilder &SwapchainBuilder::chooseSwapExtent(SDL_Window &window) {
	if (_surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		this->_extent = _surfaceCapabilities.currentExtent;
		return *this;
	}
	int width, height;
	SDL_GetWindowSizeInPixels(&window, &width, &height);
	this->_extent = vk::Extent2D{std::clamp<uint32_t>(width, _surfaceCapabilities.minImageExtent.width,
													  _surfaceCapabilities.maxImageExtent.width),
								 std::clamp<uint32_t>(height, _surfaceCapabilities.minImageExtent.height,
													  _surfaceCapabilities.maxImageExtent.height)};
	return *this;
};
SwapchainBuilder &SwapchainBuilder::chooseSwapMinImageCount() {
	auto minImageCount = std::max(3u, _surfaceCapabilities.minImageCount);
	if ((0 < _surfaceCapabilities.maxImageCount) && (_surfaceCapabilities.maxImageCount < minImageCount))
		minImageCount = _surfaceCapabilities.maxImageCount;
	this->_minImageCount = minImageCount;

	return *this;
};
SwapchainBuilder &SwapchainBuilder::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
	const auto formatIt = std::ranges::find_if(availableFormats, [](const auto &format) {
		return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
	});
	this->_format = formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
	return *this;
};

SwapchainBuilder &SwapchainBuilder::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
	assert(std::ranges::any_of(availablePresentModes,
							   [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
	this->_presentMode =
		std::ranges::any_of(availablePresentModes,
							[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; })
			? vk::PresentModeKHR::eMailbox
			: vk::PresentModeKHR::eFifo;
	return *this;
}

vk::SwapchainKHR SwapchainBuilder::buildSwapchain(vk::SurfaceKHR &surface, const vk::Device &device) {
	vk::SwapchainCreateInfoKHR swapChainCreateInfo{.surface = surface,
												   .minImageCount = this->_minImageCount,
												   .imageFormat = this->_format.format,
												   .imageColorSpace = this->_format.colorSpace,
												   .imageExtent = this->_extent,
												   .imageArrayLayers = 1,
												   .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
												   .imageSharingMode = vk::SharingMode::eExclusive,
												   .preTransform = _surfaceCapabilities.currentTransform,
												   .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
												   .presentMode = this->_presentMode,
												   .clipped = true};
	return device.createSwapchainKHR(swapChainCreateInfo);
}
} // namespace vkEngine
