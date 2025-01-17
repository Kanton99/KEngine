#include "pipeline_builder.hpp"

void Pipeline_builder::clear()
{
	_input_assembly = vk::PipelineInputAssemblyStateCreateInfo();
	_rasterization = vk::PipelineRasterizationStateCreateInfo();
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
	
}
