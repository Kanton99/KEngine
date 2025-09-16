#include <myECS/App.hpp>

namespace KEngine::myECS {
App::App()
    : m_components(std::make_unique<ComponentsManager>()), sampleVar(10) {}

} // namespace KEngine::myECS
