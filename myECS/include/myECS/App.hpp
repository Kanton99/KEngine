#ifndef ECS_APP
#define ECS_APP

#include "myECS/Component.hpp"
#include <iostream>
#include <memory>
#include <myECS/ComponentsManager.hpp>
namespace KEngine::myECS {
class App {

public:
  App(const App &) = delete;
  App(App &&) = delete;
  App &operator=(const App &) = delete;
  App &operator=(App &&) = delete;
  App();

  template <ComponentType Comp> void registerComponent();

private:
  std::unique_ptr<ComponentsManager> m_components;
  int sampleVar;
};

//------------------------------------------------------------------------------------------
template <ComponentType Comp> void App::registerComponent() {
  m_components->registerComponent<Comp>();
  std::cout << "Registered in app Component: " << typeid(Comp).name()
            << std::endl;
}
} // namespace KEngine::myECS

#endif // !ECS_APP
