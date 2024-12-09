#include <VkBootstrap.h>
#include <exception>
#include <vulkan/vulkan_core.h>
#define SDL_MAIN_HANDLED
#include "vkEngine.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>

int main() {
  try {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "Failed to init video, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_Window *window = SDL_CreateWindow(
        "test windows", 680,
        480, SDL_WINDOW_VULKAN);

    if (!window) {
      std::cerr << "Failed to create window, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }

    mvk::vkEngine *engine = mvk::vkEngine::get(window);
    engine->init();

    engine->cleanup();

    SDL_DestroyWindow(window);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
