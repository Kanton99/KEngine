#define VMA_IMPLEMENTATION
#include <vkEngine/bufferHandling.hpp>

namespace vkEngine {
BufferHandler::BufferHandler(vk::Instance instance, vk::Device device, vk::PhysicalDevice physicalDevice) {
	auto _funtions = vma::functionsFromDispatchers(VULKAN_HPP_DEFAULT_DISPATCHER);
	vma::AllocatorCreateInfo allocatorInfo{.physicalDevice = physicalDevice,
																				 .device = device,
																				 .pVulkanFunctions = &_funtions,
																				 .instance = instance,
																				 .vulkanApiVersion = vk::ApiVersion14};
	this->allocator = vma::createAllocator(allocatorInfo);
};
Buffer BufferHandler::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
																	 vk::MemoryPropertyFlags properties,
																	 vma::AllocationCreateFlags allocatorFlags = vma::AllocationCreateFlags{},
																	 vma::MemoryUsage allocatorUsage = vma::MemoryUsage::eAuto) {

	vk::BufferCreateInfo bufferInfo{.size = size, .usage = usageFlags, .sharingMode = vk::SharingMode::eExclusive};
	vma::AllocationCreateInfo allocInfo{.flags = allocatorFlags, .usage = allocatorUsage};

	auto [allocation, buffer] = this->allocator.createBuffer(bufferInfo, allocInfo);
	return {buffer, allocation};
}
void BufferHandler::copyBuffer(vk::CommandBuffer commandBuffer, vk::Queue transferQueue, vk::Buffer &srcBuffer,
															 vk::Buffer &dstBuffer, vk::DeviceSize size) {
	commandBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandBuffer.end();

	transferQueue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &commandBuffer});
	transferQueue.waitIdle();
}
void BufferHandler::uploadBufferData(vk::CommandBuffer commandBuffer, vk::Queue transferQueue, Buffer buffer,
																		 vk::DeviceSize bufferSize, void *data) {

	auto [stagingBuffer, stagingAllocation] = this->createBuffer(
			bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped);
	auto mappedStagedMemory = this->allocator.mapMemory(stagingAllocation);

	memcpy(mappedStagedMemory, data, bufferSize);
	this->allocator.unmapMemory(stagingAllocation);
	this->copyBuffer(commandBuffer, transferQueue, stagingBuffer, buffer.buffer, bufferSize);
	this->allocator.destroyBuffer(stagingBuffer, stagingAllocation);
}

void BufferHandler::deleteBuffer(Buffer buffer) { this->allocator.destroyBuffer(buffer.buffer, buffer.allocation); }
} // namespace vkEngine
