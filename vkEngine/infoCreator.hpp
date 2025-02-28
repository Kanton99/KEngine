#include <vulkan/vulkan.hpp>

namespace mvk {
  namespace utils {
    vk::ImageCreateInfo imageCreateInfo(vk::Format format, vk::ImageUsageFlags usage, vk::Extent3D extent);
    vk::ImageViewCreateInfo imageViewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags);
    vk::SemaphoreSubmitInfo semaphoreSubmitInfo(vk::PipelineStageFlags2 stageMask, vk::Semaphore semaphore);
    vk::SubmitInfo2 submitInfo(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signalSemaphore, vk::SemaphoreSubmitInfo* waitSemaphore);
    vk::CommandBufferSubmitInfo commandBufferSubmitInfo(vk::CommandBuffer cmd);
  }
}
