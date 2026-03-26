#ifndef VK_ENGINE
#define VK_ENGINE
#include <SDL3/SDL_video.h>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTOR
#include <vulkan/vulkan.hpp>
namespace vkEngine {
class vkEngine {
  public:
	vkEngine(SDL_Window *window);
	vkEngine(vkEngine &&) = default;
	vkEngine(const vkEngine &) = default;
	vkEngine &operator=(vkEngine &&) = default;
	vkEngine &operator=(const vkEngine &) = default;
	~vkEngine();

	void init();

	void draw();

	void cleanup();

  private:
};
} // namespace vkEngine
#endif
