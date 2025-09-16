#include "myECS/definitions.hpp"
#include <memory>
#include <myECS/App.hpp>

namespace KEngine {
App::App()
    : m_components(std::make_unique<ComponentsManager>()), sampleVar(10) {}

//---------------------------------------------------------------------------------
void App::createWorld() { this->m_currentWorld = std::make_unique<World>(); }

//---------------------------------------------------------------------------------
Entity App::createEntity() { return this->m_currentWorld->createEntity(); }

//---------------------------------------------------------------------------------
void App::destroyEntity(Entity entity) {
  this->m_currentWorld->destroyEntity(entity);
}
} // namespace KEngine
