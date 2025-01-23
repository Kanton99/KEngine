#include "pipeline_builder.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include "init.hpp"

void Pipeline_builder::clear()
{
	_input_assembly = vk::PipelineInputAssemblyStateCreateInfo();
	_rasterizer = vk::PipelineRasterizationStateCreateInfo();
	_color_blend_attachment = vk::PipelineColorBlendAttachmentState();
	_multisampling = vk::PipelineMultisampleStateCreateInfo();
	_pipeline_layout = vk::PipelineLayout();
	_depth_stencil = vk::PipelineDepthStencilStateCreateInfo();
	_rendering_info = vk::PipelineRenderingCreateInfo();
	_color_attachment_format = vk::Format();

	_shader_stages.clear();
}

vk::Pipeline Pipeline_builder::build_pipeline(vk::Device device)
{
  // make viewport state from our stored viewport and scissor
  // at the moment we wont support multiple viewport or scissors
  vk::PipelineViewportStateCreateInfo viewport_state{
    .viewportCount = 1,
    .scissorCount = 1
  };
  
  // setup dummy color blending.
  vk::PipelineColorBlendStateCreateInfo color_blending{
    .logicOpEnable = vk::False,
    .logicOp = vk::LogicOp::eCopy,
    .attachmentCount = 1,
    .pAttachments = &_color_blend_attachment
  };

  //clear vertexInputStateCreateInfo
  vk::PipelineVertexInputStateCreateInfo _vertex_input_info{};
  
  //build actual pipeline_builder
  vk::GraphicsPipelineCreateInfo pipeline_info{
    .pNext = &_rendering_info,

    .stageCount = (uint32_t)_shader_stages.size(),
    .pStages = _shader_stages.data(),
    .pVertexInputState = &_vertex_input_info,
    .pInputAssemblyState = &_input_assembly,
    .pViewportState = &viewport_state,
    .pRasterizationState = &_rasterizer,
    .pMultisampleState = &_multisampling,
    .pDepthStencilState = &_depth_stencil,
    .pColorBlendState = &color_blending,
    .layout = _pipeline_layout
  };

  vk::DynamicState state[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamic_info{
    .dynamicStateCount = 2,
    .pDynamicStates = &state[0]
  };

  pipeline_info.pDynamicState = &dynamic_info;

  vk::Pipeline pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info).value;

  return pipeline;
}

void Pipeline_builder::set_shaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader){
  _shader_stages.clear();

  _shader_stages.push_back(vk_init::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eVertex, vertex_shader));
  _shader_stages.push_back(vk_init::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eFragment, fragment_shader));
}

void Pipeline_builder::set_input_topology(vk::PrimitiveTopology topology){
  _input_assembly.topology = topology;

  _input_assembly.primitiveRestartEnable = vk::False;
}

void Pipeline_builder::set_polygon_mode(vk::PolygonMode mode){
  _rasterizer.polygonMode = mode;
  _rasterizer.lineWidth = 1.f;
}

void Pipeline_builder::set_cull_mode(vk::CullModeFlags cull_mode, vk::FrontFace front_face){
  _rasterizer.cullMode = cull_mode;
  _rasterizer.frontFace = front_face;
}

void Pipeline_builder::set_multisampling_none(){
  _multisampling.sampleShadingEnable = vk::False;
  _multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
  _multisampling.minSampleShading = 1.f;
  _multisampling.alphaToCoverageEnable = vk::False;
  _multisampling.alphaToOneEnable = vk::False;
}

void Pipeline_builder::disable_blending(){
  _color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  _color_blend_attachment.blendEnable = vk::False;
}

void Pipeline_builder::set_color_attachment_format(vk::Format format){
  _color_attachment_format = format;
  _rendering_info.colorAttachmentCount = 1;
  _rendering_info.pColorAttachmentFormats = &_color_attachment_format;
}

void Pipeline_builder::set_depth_format(vk::Format format){
  _rendering_info.depthAttachmentFormat = format;
}

void Pipeline_builder::disable_depthtest(){
  _depth_stencil.depthTestEnable = vk::False;
  _depth_stencil.depthWriteEnable = vk::False;
  _depth_stencil.depthCompareOp = vk::CompareOp::eNever;
  _depth_stencil.depthBoundsTestEnable = vk::False;
  _depth_stencil.stencilTestEnable = vk::False;
  _depth_stencil.front = {};
  _depth_stencil.back = {};
  _depth_stencil.minDepthBounds = 0.f;
  _depth_stencil.maxDepthBounds = 1.f;
}
