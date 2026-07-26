#ifndef VK_ENGINE_PIPELINE_BUIDLER
#define VK_ENGINE_PIPELINE_BUIDLER
#include <string>
#include <vector>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace vkEngine {
class PipelineBuilder {
  public:
	PipelineBuilder(vk::Device device) :
		_device(device) {}
	PipelineBuilder(const PipelineBuilder &) = default;
	PipelineBuilder(PipelineBuilder &&) = default;
	PipelineBuilder &operator=(const PipelineBuilder &) = delete;
	PipelineBuilder &operator=(PipelineBuilder &&) = delete;
	~PipelineBuilder() = default;

	[[nodiscard]] vk::Pipeline build();
	PipelineBuilder &loadShaderCode(const std::string &fileName);
	PipelineBuilder &createShaderModule();
	PipelineBuilder &createPipelineStage(vk::ShaderStageFlagBits stage, std::string entryPoint);
	PipelineBuilder &createDynamicStates();

  private:
	vk::Device _device;
	std::vector<char> _shaderBinary;
	vk::ShaderModule _shaderModule;
	std::vector<vk::PipelineShaderStageCreateInfo> _shaderStages;
};
} // namespace vkEngine

#endif // !VK_ENGINE_PIPELINE_BUIDLER
