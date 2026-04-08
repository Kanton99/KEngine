#ifndef VK_ENGINE
#define VK_ENGINE
#include "cleanupStruct.hpp"
#include <SDL3/SDL_video.h>
#include <memory>
#include <vector>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace vkEngine {
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif
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
	void _checkExtensions(std::vector<const char *> extensions);
	void _checkLayers(std::vector<char const *> layers);
	void _createDevice();

  private:
	vk::Instance _instance;
	std::shared_ptr<SDL_Window> _window;
	const std::vector<char const *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};

	std::unique_ptr<CleanupQueue> _cleanupQueue;
};
} // namespace vkEngine
#endif
