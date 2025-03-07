#define VULKAN_HPP_NO_CONSTRUCTORS
#include "infoCreator.hpp"

vk::ImageCreateInfo mvk::utils::imageCreateInfo(vk::Format format,
                                                vk::ImageUsageFlags usage,
                                                vk::Extent3D extent) {
  vk::ImageCreateInfo info{
      .imageType = vk::ImageType::e2D,
      .format = format,
      .extent = extent,
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = vk::ImageTiling::eOptimal,
      .usage = usage,
  };

  return info;
}

vk::ImageViewCreateInfo
mvk::utils::imageViewCreateInfo(vk::Format format, vk::Image image,
                                vk::ImageAspectFlags aspectFlags) {
  vk::ImageViewCreateInfo viewInfo{
      .image = image,
      .viewType = vk::ImageViewType::e2D,
      .format = format,
      .subresourceRange =
          {
              .aspectMask = aspectFlags,
              .levelCount = 1,
              .layerCount = 1,
          },
  };

  return viewInfo;
}

vk::SemaphoreSubmitInfo
mvk::utils::semaphoreSubmitInfo(vk::PipelineStageFlags2 stageMask,
                                vk::Semaphore semaphore) {
  return {
      .semaphore = semaphore,
      .value = 1,
      .stageMask = stageMask,
  };
}

vk::SubmitInfo2 mvk::utils::submitInfo(vk::CommandBufferSubmitInfo *cmd,
                                       vk::SemaphoreSubmitInfo *signalSemaphore,
                                       vk::SemaphoreSubmitInfo *waitSemaphore) {
  vk::SubmitInfo2 info{};
  info.setCommandBufferInfos(*cmd);
  if (waitSemaphore != nullptr)
    info.setWaitSemaphoreInfos(*waitSemaphore);
  if (signalSemaphore != nullptr)
    info.setSignalSemaphoreInfos(*signalSemaphore);
  return info;
}

vk::CommandBufferSubmitInfo
mvk::utils::commandBufferSubmitInfo(vk::CommandBuffer cmd) {
  return {
      .commandBuffer = cmd,
      .deviceMask = 0,
  };
}
