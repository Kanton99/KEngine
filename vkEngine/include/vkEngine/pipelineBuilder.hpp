#ifndef VK_ENGINE_PIPELINE_BUIDLER
#define VK_ENGINE_PIPELINE_BUIDLER
#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <vector>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace vkEngine {
struct UniformBufferObject {
	glm::mat4 model, view, proj;
};
class PipelineBuilder {
public:
	PipelineBuilder(vk::Device device) :
			_device(device) {}
	PipelineBuilder(const PipelineBuilder &) = default;
	PipelineBuilder(PipelineBuilder &&) = default;
	PipelineBuilder &operator=(const PipelineBuilder &) = delete;
	PipelineBuilder &operator=(PipelineBuilder &&) = delete;
	~PipelineBuilder() = default;

	[[nodiscard]] std::pair<vk::Pipeline, vk::PipelineLayout> build(vk::SurfaceFormatKHR &format);
	PipelineBuilder &loadShaderCode(const std::string &fileName);
	PipelineBuilder &createShaderModule();
	PipelineBuilder &createPipelineStage(vk::ShaderStageFlagBits stage, std::string entryPoint);
	PipelineBuilder &setViewPortState(vk::Rect2D viewportSize, vk::Rect2D scissorSize);
	vk::DescriptorSetLayout createDescriptorSetLayout();

private:
	vk::Device _device;
	std::vector<char> _shaderBinary;
	vk::ShaderModule _shaderModule;
	std::vector<vk::PipelineShaderStageCreateInfo> _shaderStagesInfos;
	vk::PipelineDynamicStateCreateInfo _dynamicStateInfo;
	vk::PipelineViewportStateCreateInfo _viewportState;
	vk::DescriptorSetLayout _descriptorSetLayout;
};
} // namespace vkEngine

#endif // !VK_ENGINE_PIPELINE_BUIDLER
