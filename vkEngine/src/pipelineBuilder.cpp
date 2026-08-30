#include <cstddef>
#include <cstdint>
#include <print>
#include <stdexcept>
#include <vkEngine/pipelineBuilder.hpp>
#include <vkEngine/structs/vertex.hpp>
#include <vkEngine/utils.hpp>

namespace vkEngine {
vk::Pipeline PipelineBuilder::build(vk::SurfaceFormatKHR &format) {
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
	// TODO generalize for any mesh
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescription = Vertex::getAttributeDescription();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.setVertexAttributeDescriptions(attributeDescription);
	vertexInputInfo.setVertexBindingDescriptions(bindingDescription);

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

	vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfo = {
			{.stageCount = 2,
			 .pStages = this->_shaderStagesInfos.data(),
			 .pVertexInputState = &vertexInputInfo,
			 .pInputAssemblyState = &inputAssembly,
			 .pViewportState = &this->_viewportState,
			 .pRasterizationState = &rasterizer,
			 .pMultisampleState = &multisampling,
			 .pColorBlendState = &colorBlending,
			 .pDynamicState = &this->_dynamicStateInfo,
			 .layout = layout,
			 .renderPass = nullptr},
			{.colorAttachmentCount = 1, .pColorAttachmentFormats = &format.format}};

	auto ret = this->_device.createGraphicsPipeline(nullptr, pipelineCreateInfo.get<vk::GraphicsPipelineCreateInfo>());
	if (ret.result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create graphics pipeline");
	}
	return ret.value;
}

PipelineBuilder &PipelineBuilder::loadShaderCode(const std::string &fileName) {
	std::println("Reading shader file {}", fileName);
	this->_shaderBinary = readFile(fileName);
	return *this;
}

PipelineBuilder &PipelineBuilder::createShaderModule() {
	std::println("Creating shader module");
	vk::ShaderModuleCreateInfo createInfo{.codeSize = this->_shaderBinary.size() * sizeof(char),
																				.pCode = reinterpret_cast<const uint32_t *>(this->_shaderBinary.data())};
	this->_shaderModule = this->_device.createShaderModule(createInfo);

	std::println("Created shader module");
	return *this;
}

PipelineBuilder &PipelineBuilder::createPipelineStage(vk::ShaderStageFlagBits stage, std::string entryPoint) {
	std::println("Creating pipeline {} stage", entryPoint);
	vk::PipelineShaderStageCreateInfo stageInfo{.stage = stage,
																							.module = this->_shaderModule,
																							.pName = entryPoint.data()};

	this->_shaderStagesInfos.push_back(stageInfo);

	std::println("Created pipeline {} stage", entryPoint);
	return *this;
}

PipelineBuilder &PipelineBuilder::setViewPortState(vk::Rect2D viewportSize, vk::Rect2D scissorSize) {
	std::println("Setting the viewports state");
	vk::Viewport viewport{
			0.f, 0.f, static_cast<float>(viewportSize.extent.width), static_cast<float>(viewportSize.extent.width), 0.f, 1.f};
	this->_viewportState.setViewports(viewport);
	this->_viewportState.setScissors(scissorSize);
	std::println("Set the viewports state");
	return *this;
}

} // namespace vkEngine
