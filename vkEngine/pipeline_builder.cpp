#include "pipeline_builder.hpp"
#include <cstdint>
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "init.hpp"

void PipelineBuilder::clear()
{
	_inputAssembly = vk::PipelineInputAssemblyStateCreateInfo();
	_rasterizer = vk::PipelineRasterizationStateCreateInfo();
	_colorBlendAttachment = vk::PipelineColorBlendAttachmentState();
	_multisampling = vk::PipelineMultisampleStateCreateInfo();
	_pipelineLayout = vk::PipelineLayout();
	_depthStencil = vk::PipelineDepthStencilStateCreateInfo();
	_renderingInfo = vk::PipelineRenderingCreateInfo();
	_colorAttachmentFormat = vk::Format();
  _renderPass = vk::RenderPass();

	_shaderStages.clear();
}

vk::Pipeline PipelineBuilder::buildPipeline(vk::Device device)
{
  // make viewport state from our stored viewport and scissor
  // at the moment we wont support multiple viewport or scissors
  vk::PipelineViewportStateCreateInfo viewportState{
    .viewportCount = 1,
    .scissorCount = 1
  };
  
  // setup dummy color blending.
  vk::PipelineColorBlendStateCreateInfo colorBlending{
    .logicOpEnable = vk::False,
    .logicOp = vk::LogicOp::eCopy,
    .attachmentCount = 1,
    .pAttachments = &_colorBlendAttachment
  };

  vk::VertexInputBindingDescription bindingDescriptor{
    .binding = 0,
    .stride = sizeof(glm::vec3),
    .inputRate = vk::VertexInputRate::eVertex,
  };

  vk::VertexInputAttributeDescription attributeDescriptions{
    .location = 0,
    .binding = 0,
    .format = vk::Format::eR32G32B32Sfloat,
    .offset = 0,
  };
  //clear vertexInputStateCreateInfo
  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &bindingDescriptor,
    .vertexAttributeDescriptionCount = 1,
    .pVertexAttributeDescriptions = &attributeDescriptions,
  };
  
  //build actual pipeline_builder
  vk::GraphicsPipelineCreateInfo pipelineInfo{
    .pNext = &_renderingInfo,

    .stageCount = (uint32_t)_shaderStages.size(),
    .pStages = _shaderStages.data(),
    .pVertexInputState = &vertexInputInfo,
    .pInputAssemblyState = &_inputAssembly,
    .pViewportState = &viewportState,
    .pRasterizationState = &_rasterizer,
    .pMultisampleState = &_multisampling,
    .pDepthStencilState = &_depthStencil,
    .pColorBlendState = &colorBlending,
    .layout = _pipelineLayout,
    .subpass = 0
  };

  vk::DynamicState state[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamic_info{
    .dynamicStateCount = 2,
    .pDynamicStates = &state[0]
  };

  pipelineInfo.pDynamicState = &dynamic_info;

  vk::Pipeline pipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo).value;

  return pipeline;
}

void PipelineBuilder::setShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader){
  _shaderStages.clear();

  _shaderStages.push_back(vkInit::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eVertex, vertexShader));
  _shaderStages.push_back(vkInit::pipeline_shader_stage_create_info(vk::ShaderStageFlagBits::eFragment, fragmentShader));
}

void PipelineBuilder::setInputTopology(vk::PrimitiveTopology topology){
  _inputAssembly.topology = topology;

  _inputAssembly.primitiveRestartEnable = vk::False;
}

void PipelineBuilder::setPolygonMode(vk::PolygonMode mode){
  _rasterizer.polygonMode = mode;
  _rasterizer.lineWidth = 1.f;
}

void PipelineBuilder::setCullMode(vk::CullModeFlags cullMode, vk::FrontFace frontFace){
  _rasterizer.cullMode = cullMode;
  _rasterizer.frontFace = frontFace;
}

void PipelineBuilder::setMultisamplingNone(){
  _multisampling.sampleShadingEnable = vk::False;
  _multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
  _multisampling.minSampleShading = 1.f;
  _multisampling.alphaToCoverageEnable = vk::False;
  _multisampling.alphaToOneEnable = vk::False;
}

void PipelineBuilder::disableBlending(){
  _colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
  _colorBlendAttachment.blendEnable = vk::False;
}

void PipelineBuilder::setColorAttachmentFormat(vk::Format format){
  _colorAttachmentFormat = format;
  _renderingInfo.colorAttachmentCount = 1;
  _renderingInfo.pColorAttachmentFormats = &_colorAttachmentFormat;
}

void PipelineBuilder::setDepthFormat(vk::Format format){
  _renderingInfo.depthAttachmentFormat = format;
}

void PipelineBuilder::disableDepthtest(){
  _depthStencil.depthTestEnable = vk::False;
  _depthStencil.depthWriteEnable = vk::False;
  _depthStencil.depthCompareOp = vk::CompareOp::eNever;
  _depthStencil.depthBoundsTestEnable = vk::False;
  _depthStencil.stencilTestEnable = vk::False;
  _depthStencil.front = {};
  _depthStencil.back = {};
  _depthStencil.minDepthBounds = 0.f;
  _depthStencil.maxDepthBounds = 1.f;
}

void PipelineBuilder::enableDepthtest(bool depthWriteEnable, vk::CompareOp op)
{
    _depthStencil.depthTestEnable = VK_TRUE;
    _depthStencil.depthWriteEnable = depthWriteEnable;
    _depthStencil.depthCompareOp = op;
    _depthStencil.depthBoundsTestEnable = VK_FALSE;
    _depthStencil.stencilTestEnable = VK_FALSE;
    _depthStencil.minDepthBounds = 0.f;
    _depthStencil.maxDepthBounds = 1.f;
}


void PipelineBuilder::setRenderPass(vk::Device device){
  vk::AttachmentDescription colorAttachment{
    .format = _colorAttachmentFormat,
    .samples = vk::SampleCountFlagBits::e1,

    .loadOp = vk::AttachmentLoadOp::eClear,
    .storeOp = vk::AttachmentStoreOp::eStore,

    .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
    .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,

    .initialLayout = vk::ImageLayout::eUndefined,
    .finalLayout = vk::ImageLayout::ePresentSrcKHR
  };

  vk::AttachmentReference colorAttachmentRef{
    .attachment = 0,
    .layout = vk::ImageLayout::eColorAttachmentOptimal
  };

  vk::SubpassDescription subpass{
    .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachmentRef
  };

  std::vector<vk::AttachmentDescription> attachments = {colorAttachment};
  vk::RenderPassCreateInfo renderPassInfo{
    .pAttachments = attachments.data(),
    .subpassCount = 1,
    .pSubpasses = &subpass,
  };

  _renderPass = device.createRenderPass(renderPassInfo);
}
