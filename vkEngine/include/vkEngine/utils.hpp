#ifndef VK_ENGINE_UTILS_HPP
#define VK_ENGINE_UTILS_HPP

#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
namespace vkEngine {
static std::vector<char> readFile(const std::string &fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	std::vector<char> buffer(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close();

	return buffer;
}

inline void transitionImageLayout(vk::Image &image, vk::CommandBuffer &commandBuffer, vk::ImageLayout oldLayout,
								  vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask,
								  vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
								  vk::PipelineStageFlags2 dstStageMask) {
	vk::ImageMemoryBarrier2 barrier{.srcStageMask{srcStageMask},
									.srcAccessMask{srcAccessMask},
									.dstStageMask{dstStageMask},
									.dstAccessMask{dstAccessMask},
									.oldLayout = oldLayout,
									.newLayout = newLayout,
									.srcQueueFamilyIndex = vk::QueueFamilyIgnored,
									.dstQueueFamilyIndex = vk::QueueFamilyIgnored,
									.image{image},
									.subresourceRange{.aspectMask{vk::ImageAspectFlagBits::eColor},
													  .baseMipLevel = 0,
													  .levelCount = 1,
													  .baseArrayLayer = 0,
													  .layerCount = 1}};
	vk::DependencyInfo dependencyInfo{
		.dependencyFlags{},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
	};
	commandBuffer.pipelineBarrier2(dependencyInfo);
}
} // namespace vkEngine
#endif // !VK_ENGINE_UTILS_HPP
