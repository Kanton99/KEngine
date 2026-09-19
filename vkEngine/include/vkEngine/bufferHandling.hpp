#ifndef VKENGINE_BUFFER_HANDLING
#define VKENGINE_BUFFER_HANDLING
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VK_NO_PROTOTYPES
#include <vk_mem_alloc.hpp>
namespace vkEngine {
struct Buffer {
	vk::Buffer buffer;
	vma::Allocation allocation;
};
struct BARBuffer : Buffer {
	void *mappedMemory;
};

class BufferHandler {
private:
	vma::Allocator allocator;
	void copyBuffer(vk::CommandBuffer commandBuffe, vk::Queue transferQueue, vk::Buffer &srcBuffer, vk::Buffer &dstBuffer,
									vk::DeviceSize size);

public:
	BufferHandler() :
			allocator{} {}
	BufferHandler(vk::Instance instance, vk::Device device, vk::PhysicalDevice physicalDevice);
	Buffer createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags properties,
											vma::AllocationCreateFlags allocatorFlags, vma::MemoryUsage allocatorUsage);
	void uploadBufferDataStaged(vk::CommandBuffer commandBuffer, vk::Queue transferQueue, Buffer buffer,
															vk::DeviceSize bufferSize, void *data);
	BARBuffer createBARBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags properties,
														vma::AllocationCreateFlags allocatorFlags, vma::MemoryUsage allocatorUsage);
	void uploadBARBufferData(vk::CommandBuffer commandBuffer, vk::Queue transferQueue, BARBuffer buffer,
													 vk::DeviceSize bufferSize, void *data);

	void deleteBuffer(Buffer buffer);
};
} // namespace vkEngine

#endif // !VKENGINE_BUFFER_HANDLING
