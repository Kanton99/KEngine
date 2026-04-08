#ifndef VK_ENGINE
#define VK_ENGINE
#include "cleanupStruct.hpp"
#include <SDL3/SDL_video.h>
#include <memory>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "volk.h"
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace vkEngine {
class vkEngine {
  public:
	vkEngine(std::shared_ptr<SDL_Window> window);
	vkEngine(vkEngine &&) = delete;
	vkEngine(const vkEngine &) = delete;
	vkEngine &operator=(vkEngine &&) = delete;
	vkEngine &operator=(const vkEngine &) = delete;
	~vkEngine() = default;

	void init();

	void draw();

	void cleanup();

  private:
	void _createInstance();

  private:
	vk::Instance _instance;
	std::shared_ptr<SDL_Window> _window;

	std::unique_ptr<CleanupQueue> _cleanupQueue;
};
} // namespace vkEngine
#endif
