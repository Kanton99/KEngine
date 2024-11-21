#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <exception>
#include <vulkan/vulkan_core.h>
#define SDL_MAIN_HANDLED
#include "vkEngine.h"
#include <SDL.h>
#include <iostream>

int main() {
  try {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "Failed to init video, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "test windows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 680,
        480, SDL_WindowFlags(SDL_WINDOW_VULKAN));

    if (!window) {
      std::cerr << "Failed to create window, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }
    //
    // SDL_Surface* window_surface = SDL_GetWindowSurface(window);

    // if (!window_surface)
    //{
    //	std::cout << "Failed to get the surface from the window\n";
    //	return -1;
    // }
    /**/
    mvk::vkEngine *engine = mvk::vkEngine::get(window);
    engine->init();
    int i;
    std::cin >> i;
    engine->cleanup();

    SDL_DestroyWindow(window);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
