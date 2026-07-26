#include <cstdint>
#include <vkEngine/pipelineBuilder.hpp>
#include <vkEngine/utils.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace vkEngine {
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

PipelineBuilder &PipelineBuilder::createDynamicStates() {
	std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	return *this;
}
} // namespace vkEngine
