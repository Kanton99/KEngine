/*#include "gltfLoader.hpp"*/
#include "gltfLoader.hpp"
#include "objLoader.hpp"
#include <SDL3/SDL_system.h>
#include <array>
#include <cstdint>
#include <exception>
#include <glm/fwd.hpp>
#define SDL_MAIN_HANDLED
#include "vkEngine.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>

int main() {
  try {
    // setenv("SDL_VIDEODRIVER", "x11", 1);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
      std::cerr << "Failed to init video, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_Window *window =
        SDL_CreateWindow("test windows", 1600, 900, SDL_WINDOW_VULKAN);

    if (!window) {
      std::cerr << "Failed to create window, Error: " << SDL_GetError()
                << std::endl;
      return -1;
    }

    mvk::vkEngine *engine = mvk::vkEngine::get(window);
    engine->init();

    std::array<glm::vec3, 8> quad{
        glm::vec3(-1, 1, 1),   glm::vec3(1, 1, 1),   glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),   glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1),
        glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1),
    };
    std::array<unsigned int, 3 * 2 * 6> indeces{
        0, 1, 2, 2, 1, 3, 5, 1, 7, 7, 1, 3, 4, 5, 6, 6, 5, 7,
        0, 1, 4, 4, 1, 5, 0, 4, 2, 2, 4, 6, 6, 7, 2, 2, 7, 3};
    std::array<glm::vec3, 8> normals;
    std::array<glm::vec2, 8> uvs;
    std::array<glm::vec4, 8> colors = {
        glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 1, 1), glm::vec4(0, 1, 0, 1),
        glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1),
        glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1),
    };

    std::array<mvk::VertexData, 8> vectors;

    for (size_t i = 0; i < 8; i++) {
      vectors[i].normals = normals[i];
      vectors[i].colour = colors[i];
      vectors[i].position = quad[i];
      vectors[i].uv_x = uvs[i].x;
      vectors[i].uv_y = uvs[i].y;
    }

    /*engine->testMeshes = mvk::loadGltfMesh(engine, "resources/Meshes/basicmesh.glb").value();*/
    engine->testMeshes = mvk::loadObj(engine, "resources/Meshes/teapot.obj").value();

    SDL_Event event;
    bool running = true;

    while (running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
      }
      engine->draw();
      SDL_Delay(static_cast<uint32_t>(1.f / 60.f) * 1000);
    }

    engine->cleanup();

    SDL_DestroyWindow(window);
    SDL_Quit();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
