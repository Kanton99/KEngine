#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
class Pipeline_builder {
public:
	std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages;

	vk::PipelineInputAssemblyStateCreateInfo _input_assembly;
	vk::PipelineRasterizationStateCreateInfo _rasterizer;
	vk::PipelineColorBlendAttachmentState _color_blend_attachment;
	vk::PipelineMultisampleStateCreateInfo _multisampling;
	vk::PipelineLayout _pipeline_layout;
	vk::PipelineDepthStencilStateCreateInfo _depth_stencil;
	vk::PipelineRenderingCreateInfo _rendering_info;
	vk::Format _color_attachment_format;
  vk::RenderPass _render_pass;
  //TODO Add vertex input

	Pipeline_builder() { clear(); }

	void clear();

	vk::Pipeline build_pipeline(vk::Device device);

  void set_shaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader);
  void set_input_topology(vk::PrimitiveTopology topology);
  void set_polygon_mode(vk::PolygonMode mode);
  void set_cull_mode(vk::CullModeFlags cull_mode, vk::FrontFace front_face);
  void set_multisampling_none();
  void disable_blending();
  void set_color_attachment_format(vk::Format format);
  void set_depth_format(vk::Format format);
  void disable_depthtest();
  void set_render_pass(vk::Device device);
};
