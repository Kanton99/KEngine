#ifndef KENGINE_MAIN
#define KENGINE_MAIN

#include "myECS/SystemManager.hpp"
#include <SDL3/SDL_system.h>
#include <exception>
#include <vkEngine/gltfLoader.hpp>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <vkEngine/vkEngine.hpp>

#include <myECS/App.hpp>
#include <myECS/Component.hpp>

struct Trasform : public KEngine::myECS::Component<Trasform> {
  glm::mat4 transform;
};
struct Player : public KEngine::myECS::Component<Player> {};
struct Enemy : public KEngine::myECS::Component<Enemy> {};

int main() {
  try {
    KEngine::myECS::App app;
    app.registerComponent<Trasform>();
    app.registerComponent<Player>();
    app.registerComponent<Enemy>();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

#endif
