#include <string>
#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace mvk {
namespace utils {
std::vector<char> readFile(const std::string &filename);

vk::ShaderModule createShaderModule(const std::vector<char> shader,
                                    const vk::Device device);

void transitionImage(vk::CommandBuffer cmd, vk::Image image,
                     vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

void copyImageToImage(vk::CommandBuffer cmd, vk::Image source,
                      vk::Image destination, vk::Extent2D srcSize,
                      vk::Extent2D dstSize);
} // namespace utils
} // namespace mvk
