#include <vkEngine/DescriptorLayoutBuilder.hpp>

void mvk::DescriptorLayoutBuidler::addBinding(uint32_t binding, vk::DescriptorType type){
  vk::DescriptorSetLayoutBinding newbind;
  newbind.setBinding(binding);
  newbind.setDescriptorCount(1);
  newbind.setDescriptorType(type);

  this->bindings.push_back(newbind);
}

void mvk::DescriptorLayoutBuidler::clear(){
  bindings.clear();
}

    vk::DescriptorSetLayout mvk::DescriptorLayoutBuidler::build(vk::Device device, vk::ShaderStageFlags shaderStage, void* pNext, vk::DescriptorSetLayoutCreateFlags flags){
  for(auto& b : this->bindings) b.stageFlags |= shaderStage;

  vk::DescriptorSetLayoutCreateInfo createInfo;
  createInfo.setPNext(pNext);
  createInfo.setBindings(this->bindings);
  createInfo.setFlags(flags);

  vk::DescriptorSetLayout set = device.createDescriptorSetLayout(createInfo);
  return set;
}
