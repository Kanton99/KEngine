#include <SDL3/SDL_system.h>
#include <exception>
#define SDL_MAIN_HANDLED
#include "vkEngine.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_hints.h>
#include <iostream>

int main() {
  try {
    setenv("SDL_VIDEODRIVER", "x11", 1);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "Failed to init video, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_Window *window = SDL_CreateWindow(
        "test windows", 1600,
        900, SDL_WINDOW_VULKAN);

    if (!window) {
      std::cerr << "Failed to create window, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }

    mvk::vkEngine *engine = mvk::vkEngine::get(window);
    engine->init();

    SDL_Event event;
    bool running = true;

    while(running){
      while(SDL_PollEvent(&event)){
        if(event.type == SDL_EVENT_QUIT) running = false;
      }
      engine->draw();
      SDL_Delay((1.f/60.f)*1000);
    }

    engine->cleanup();

    SDL_DestroyWindow(window);
    SDL_Quit();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
