#ifndef ECS_COMPONENT_STORAGE_HPP
#define ECS_COMPONENT_STORAGE_HPP

#include <concepts>
#include <myECS/Component.hpp>
#include <vector>
namespace KEngine {

class IComponentStorage {
public:
  virtual ~IComponentStorage() = default;
};

template <class Comp>
  requires std::derived_from<Comp, Component<Comp>>
class ComponentStorage : public IComponentStorage {
public:
private:
  std::vector<Component<Comp>> m_components;
};

} // namespace KEngine

#endif // !ECS_COMPONENT_STORAGE_HPP
