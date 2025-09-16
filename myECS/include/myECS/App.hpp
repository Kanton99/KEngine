#ifndef ECS_APP
#define ECS_APP

#include "myECS/Component.hpp"
#include "myECS/World.hpp"
#include "myECS/definitions.hpp"
#include <iostream>
#include <memory>
#include <myECS/ComponentsManager.hpp>
namespace KEngine {
class App {

public:
  App(const App &) = delete;
  App(App &&) = delete;
  App &operator=(const App &) = delete;
  App &operator=(App &&) = delete;
  App();

  template <ComponentType Comp> void registerComponent();

  void createWorld();

  Entity createEntity();
  void destroyEntity(Entity entity);

private:
  std::unique_ptr<ComponentsManager> m_components;
  int sampleVar;

  std::vector<std::unique_ptr<World>> m_worlds;
  std::unique_ptr<World> m_currentWorld;
};

//------------------------------------------------------------------------------------------
template <ComponentType Comp> void App::registerComponent() {
  m_components->registerComponent<Comp>();
  std::cout << "Registered in app Component: " << typeid(Comp).name()
            << std::endl;
}
} // namespace KEngine

#endif // !ECS_APP
