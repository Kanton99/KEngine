#include <vulkan/vulkan.hpp>

namespace mvk {
vk::ImageCreateInfo imageCreateInfo(vk::Format format, vk::ImageUsageFlags usage, vk::Extent3D extent);
}
