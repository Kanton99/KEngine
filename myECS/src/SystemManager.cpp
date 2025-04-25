#include <memory>
#include <myECS/SystemManager.hpp>

void KEngine::myECS::SystemManager::addSystem(std::unique_ptr<System> system) {
  this->m_systems.push_back(std::move(system));
}

void KEngine::myECS::SystemManager::startSystems() const {
  for (const auto &system : m_systems) {
    system->start();
  }
}

void KEngine::myECS::SystemManager::updateSystems() const {
  for (const auto &system : m_systems) {
    system->update();
  }
}
