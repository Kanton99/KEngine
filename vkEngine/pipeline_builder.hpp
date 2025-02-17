#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
class PipelineBuilder {
public:
	std::vector<vk::PipelineShaderStageCreateInfo> _shaderStages;

	vk::PipelineInputAssemblyStateCreateInfo _inputAssembly;
	vk::PipelineRasterizationStateCreateInfo _rasterizer;
	vk::PipelineColorBlendAttachmentState _colorBlendAttachment;
	vk::PipelineMultisampleStateCreateInfo _multisampling;
	vk::PipelineLayout _pipelineLayout;
	vk::PipelineDepthStencilStateCreateInfo _depthStencil;
	vk::PipelineRenderingCreateInfo _renderingInfo;
	vk::Format _colorAttachmentFormat;
  vk::RenderPass _renderPass;
  //TODO Add vertex input

	PipelineBuilder() { clear(); }

	void clear();

	vk::Pipeline buildPipeline(vk::Device device);

  void setShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader);
  void setInputTopology(vk::PrimitiveTopology topology);
  void setPolygonMode(vk::PolygonMode mode);
  void setCullMode(vk::CullModeFlags cullMode, vk::FrontFace frontFace);
  void setMultisamplingNone();
  void disableBlending();
  void setColorAttachmentFormat(vk::Format format);
  void setDepthFormat(vk::Format format);
  void disableDepthtest();
  void setRenderPass(vk::Device device);
};
