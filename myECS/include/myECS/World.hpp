#ifndef ECS_WORLD_HPP
#define ECS_WORLD_HPP

#include <myECS/definitions.hpp>
#include <set>
namespace KEngine {

class World {
public:
  World(const World &) = delete;
  World(World &&) = delete;
  World &operator=(const World &) = delete;
  World &operator=(World &&) = delete;
  World();

  Entity createEntity();
  void destroyEntity(Entity entity);

private:
  std::set<Entity> m_entities;
  std::set<Entity> m_freeEntityIds;
};

} // namespace KEngine

#endif // !ECS_WORLD_HPP
