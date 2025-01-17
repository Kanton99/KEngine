#include <vector>
#include <vulkan/vulkan.hpp>
class Pipeline_builder {
public:
	std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages;

	vk::PipelineInputAssemblyStateCreateInfo _input_assembly;
	vk::PipelineRasterizationStateCreateInfo _rasterization;
	vk::PipelineColorBlendAttachmentState _color_blend_attachment;
	vk::PipelineMultisampleStateCreateInfo _multisampling;
	vk::PipelineLayout _pipeline_layout;
	vk::PipelineDepthStencilStateCreateInfo _depth_stencil;
	vk::PipelineRenderingCreateInfo _rendering_info;
	vk::Format _color_attachment_format;

	Pipeline_builder() { clear(); }

	void clear();

	vk::Pipeline build_pipeline(vk::Device device);
};