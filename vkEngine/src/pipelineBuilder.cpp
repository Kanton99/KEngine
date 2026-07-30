#include <cstddef>
#include <cstdint>
#include <vkEngine/pipelineBuilder.hpp>
#include <vkEngine/utils.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace vkEngine {
vk::Pipeline PipelineBuilder::build() {
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
	vk::PipelineRasterizationStateCreateInfo rasterizer{.depthClampEnable = vk::False,
														.rasterizerDiscardEnable = vk::False,
														.polygonMode = vk::PolygonMode::eFill,
														.cullMode = vk::CullModeFlagBits::eBack,
														.frontFace = vk::FrontFace::eClockwise,
														.depthBiasEnable = vk::False,
														.lineWidth = 1.f};
	vk::PipelineMultisampleStateCreateInfo multisampling{.rasterizationSamples = vk::SampleCountFlagBits::e1,
														 .sampleShadingEnable = vk::False};

	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
		.blendEnable = vk::True,
		.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
		.dstColorBlendFactor = vk::BlendFactor::eOneMinusDstAlpha,
		.colorBlendOp = vk::BlendOp::eAdd,
		.srcAlphaBlendFactor = vk::BlendFactor::eOne,
		.dstAlphaBlendFactor = vk::BlendFactor::eZero,
		.alphaBlendOp = vk::BlendOp::eAdd,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
						  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
	};
	vk::PipelineColorBlendStateCreateInfo colorBlending{
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
	};

	std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
	this->_dynamicStateInfo.setDynamicStates(dynamicStates);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};
	vk::PipelineLayout layout = this->_device.createPipelineLayout(pipelineLayoutInfo);
	// TODO return actual pipeline
	return vk::Pipeline{};
}

PipelineBuilder &PipelineBuilder::loadShaderCode(const std::string &fileName) {
	this->_shaderBinary = readFile(fileName);
	return *this;
}

PipelineBuilder &PipelineBuilder::createShaderModule() {
	vk::ShaderModuleCreateInfo createInfo{.codeSize = this->_shaderBinary.size() * sizeof(char),
										  .pCode = reinterpret_cast<const uint32_t *>(this->_shaderBinary.data())};
	this->_shaderModule = this->_device.createShaderModule(createInfo);

	return *this;
}

PipelineBuilder &PipelineBuilder::createPipelineStage(vk::ShaderStageFlagBits stage, std::string entryPoint) {
	vk::PipelineShaderStageCreateInfo stageInfo{.stage = stage,
												.module = this->_shaderModule,
												.pName = entryPoint.data()};

	this->_shaderStages.push_back(stageInfo);

	return *this;
}

PipelineBuilder &PipelineBuilder::setViewPortState(vk::Rect2D viewportSize, vk::Rect2D scissorSize) {
	vk::Viewport viewport{
		0.f, 0.f, static_cast<float>(viewportSize.extent.width), static_cast<float>(viewportSize.extent.width),
		0.f, 1.f};
	this->_viewportState.setViewports(viewport);
	this->_viewportState.setScissors(scissorSize);
	return *this;
}

} // namespace vkEngine
