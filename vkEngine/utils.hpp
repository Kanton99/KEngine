#include <vector>
#include <string>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

std::vector<char> readFile(const std::string &filename);

vk::ShaderModule createShaderModule(const std::vector<char> shader,
                                    const vk::Device device);

void transitionImage(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout oldLayout,
                     vk::ImageLayout newLayout);
