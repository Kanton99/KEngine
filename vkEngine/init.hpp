#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace vkInit{
  vk::PipelineShaderStageCreateInfo pipeline_shader_stage_create_info(vk::ShaderStageFlagBits stage, vk::ShaderModule shader_module, const char * entry = "main");
  vk::PipelineLayoutCreateInfo pipeline_layout_create_info(const std::span<vk::DescriptorSetLayout>& descritorLayouts);
} // namespace vk_init
