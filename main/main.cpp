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

struct Trasform : public KEngine::Component<Trasform> {
  glm::mat4 transform;
};
struct Player : public KEngine::Component<Player> {};
struct Enemy : public KEngine::Component<Enemy> {};

int main() {
  try {
    KEngine::App app;

    app.createWorld();

    KEngine::Entity ent1 = app.createEntity();

    KEngine::Entity ent2 = app.createEntity();

    app.destroyEntity(ent1);

    app.createEntity();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

#endif
