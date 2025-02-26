#include <vulkan/vulkan.hpp>

namespace mvk {
  namespace utils {
    vk::ImageCreateInfo imageCreateInfo(vk::Format format, vk::ImageUsageFlags usage, vk::Extent3D extent);
    vk::ImageViewCreateInfo imageViewCreateInfo(vk::Format format, vk::Image image, vk::ImageAspectFlags aspectFlags);
  }
}
