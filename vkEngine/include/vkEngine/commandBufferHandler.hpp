#ifndef COMMAND_BUFFER_HANDLER_HPP
#define COMMAND_BUFFER_HANDLER_HPP

#include <cstdint>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
namespace vkEngine {
class CommandBufferHandler {

  public:
	CommandBufferHandler() = default;
	CommandBufferHandler(const CommandBufferHandler &) = default;
	CommandBufferHandler(CommandBufferHandler &&) = default;
	CommandBufferHandler &operator=(const CommandBufferHandler &) = default;
	CommandBufferHandler &operator=(CommandBufferHandler &&) = default;

	void createCommandPool(vk::Device &device, uint32_t queueIndex);
	vk::CommandBuffer allocateCommandBuffer(vk::Device &device, vk::CommandBufferLevel level);

  private:
	vk::CommandPool _commandPool;
};
} // namespace vkEngine
//

#endif // !COMMAND_BUFFER_HANDLER_HPP
