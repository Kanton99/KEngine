#ifndef ECS_COMPONENT_MANAGER_HPP
#define ECS_COMPONENT_MANAGER_HPP

#include "myECS/definitions.hpp"
#include <memory>
#include <myECS/ComponentStorage.hpp>
#include <unordered_map>
namespace KEngine::myECS {

class ComponentsManager {
public:
  ComponentsManager(const ComponentsManager &) = default;
  ComponentsManager(ComponentsManager &&) = default;
  ComponentsManager &operator=(const ComponentsManager &) = default;
  ComponentsManager &operator=(ComponentsManager &&) = default;
  ComponentsManager() = default;

  template <ComponentType Comp> void registerComponent();

private:
  std::unordered_map<Signature, std::unique_ptr<IComponentStorage>> m_storages;
};

template <ComponentType Comp> void ComponentsManager::registerComponent() {
  this->m_storages.emplace(Comp::signature,
                           std::make_unique<ComponentStorage<Comp>>());
}
} // namespace KEngine::myECS

#endif // !ECS_COMPONENT_MANAGER_HPP
