#include <vkEngine/commandBufferHandler.hpp>
namespace vkEngine {
void CommandBufferHandler::createCommandPool(vk::Device &device, uint32_t queueIndex) {
	vk::CommandPoolCreateInfo poolInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queueIndex,
	};

	this->_commandPool = device.createCommandPool(poolInfo);
}
vk::CommandBuffer CommandBufferHandler::allocateCommandBuffer(vk::Device &device, vk::CommandBufferLevel level) {
	vk::CommandBufferAllocateInfo allocInfo{
		.commandPool = this->_commandPool,
		.level = level,
		.commandBufferCount = 1,
	};
	return device.allocateCommandBuffers(allocInfo).front();
}
} // namespace vkEngine
