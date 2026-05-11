#include <algorithm>
#include <vkEngine/swapchainBuilder.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace vkEngine {

void SwapchainBuilder::chooseSwapExtent() {};
void SwapchainBuilder::chooseSwapMinImageCount() {};
void SwapchainBuilder::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
	const auto formatIt = std::ranges::find_if(availableFormats, [](const auto &format) {
		return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
	});
	this->_format = formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
};

void SwapchainBuilder::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
	assert(std::ranges::any_of(availablePresentModes,
							   [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
	this->_presentMode =
		std::ranges::any_of(availablePresentModes,
							[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; })
			? vk::PresentModeKHR::eMailbox
			: vk::PresentModeKHR::eFifo;
}
} // namespace vkEngine
