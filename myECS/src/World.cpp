#include <format>
#include <myECS/World.hpp>
#include <stdexcept>

namespace KEngine {
World::World() : m_freeEntityIds({1}) {}

//-----------------------------------------------------------------------------------
Entity World::createEntity() {
  auto newEntityIt = m_freeEntityIds.begin();
  Entity entity = *newEntityIt;
  m_freeEntityIds.erase(newEntityIt);
  m_freeEntityIds.insert(entity + 1);

  m_entities.insert(entity);

  return entity;
}

//-----------------------------------------------------------------------------------
void World::destroyEntity(Entity entity) {
  auto entityIt = m_entities.find(entity);
  if (entityIt != m_entities.end()) {
    m_freeEntityIds.insert(*entityIt);
    m_entities.erase(entityIt);
  } else {
    throw std::invalid_argument(
        std::format("Entity: %d doesn't exist", entity));
  }
}
} // namespace KEngine
