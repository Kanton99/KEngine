#ifndef VKENGINE_BUFFER_HANDLING
#define VKENGINE_BUFFER_HANDLING

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>
namespace vkEngine {
struct Buffer {
	vk::Buffer buffer;
	vma::Allocation allocation;
};

class BufferHandler {};
} // namespace vkEngine

#endif // !VKENGINE_BUFFER_HANDLING
