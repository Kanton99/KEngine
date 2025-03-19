#ifndef MVK_DESCRIPTOR_LAYOUT_BUILDER
#define MVK_DESCRIPTOR_LAYOUT_BUILDER

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vector>

namespace mvk{
  struct DescriptorLayoutBuidler{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    void addBinding(uint32_t binding, vk::DescriptorType type);
    void clear();
    vk::DescriptorSetLayout build(vk::Device device, vk::ShaderStageFlags shaderStage, void* pNext = nullptr, vk::DescriptorSetLayoutCreateFlags flags = vk::DescriptorSetLayoutCreateFlags());
  };
}

#endif // MVK_DESCRIPTOR_LAYOUT_BUILDER
