#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
namespace vkEngine {
struct FrameInFlight {
	vk::Semaphore presentComplete;
	vk::Semaphore renderFinishedSemaphore;
	vk::Fence drawFence;
	vk::CommandBuffer commandBuffer;

	void cleanup(vk::Device &device) {
		device.destroy(presentComplete);
		device.destroy(renderFinishedSemaphore);
		device.destroy(drawFence);
	}
};
} // namespace vkEngine
