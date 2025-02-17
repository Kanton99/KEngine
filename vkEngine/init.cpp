#include "init.hpp"

vk::PipelineShaderStageCreateInfo vkInit::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits stage, vk::ShaderModule shader_module, const char * entry){
  vk::PipelineShaderStageCreateInfo info{
    .stage = stage,
    .module = shader_module,
    .pName = entry
  };

  return info;
}

vk::PipelineLayoutCreateInfo vkInit::pipeline_layout_create_info(){
  vk::PipelineLayoutCreateInfo info{
    .setLayoutCount = 0,
    .pSetLayouts = nullptr,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges = nullptr
  };
  return info;
}
