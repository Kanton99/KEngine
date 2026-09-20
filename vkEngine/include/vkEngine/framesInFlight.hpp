#include "vkEngine/bufferHandling.hpp"
#include <vulkan/vulkan.hpp>
namespace vkEngine {
struct FrameInFlight {
	vk::Semaphore presentComplete;
	vk::Semaphore renderFinishedSemaphore;
	vk::Fence drawFence;
	vk::CommandBuffer commandBuffer;

	BARBuffer uniformBuffer;

	void cleanup(vk::Device &device) {
		device.destroy(presentComplete);
		device.destroy(renderFinishedSemaphore);
		device.destroy(drawFence);
	}
};
} // namespace vkEngine
