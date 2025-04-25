#ifndef KENGINE_SYSTEM_MANAGER_HPP
#define KENGINE_SYSTEM_MANAGER_HPP

#include "System.hpp"
#include <memory>
#include <vector>

namespace KEngine::myECS {
class SystemManager {
private: // Members
  std::vector<std::unique_ptr<System>> m_systems;

public: // Methods
  void startSystems() const;
  void updateSystems() const;

  void addSystem(std::unique_ptr<System> system);
};
} // namespace KEngine

#endif // !KENGINE_SYSTEM_HPP
