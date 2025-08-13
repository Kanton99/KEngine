#ifndef KENGINE_SYSTEM_MANAGER_HPP
#define KENGINE_SYSTEM_MANAGER_HPP

#include "System.hpp"
#include <memory>
#include <vector>

namespace KEngine::myECS {
enum UpdateSteps { PREUPDATE, UPDATE, POSTUPDATE };
/**
System Manager class

Registers functions as systems, provided they are setup correctly. (only queries for components)

Stores systems in groups defined by UpdateSteps.

all systems are executed sequentially per step.
**/
class SystemManager {
  private: // Members
	SystemManager();
	std::vector<std::unique_ptr<System>> m_updateSystems;
	std::vector<std::unique_ptr<System>> m_preUpdateSystems;
	std::vector<std::unique_ptr<System>> m_postUpdateSystems;

  public: // Methods
	std::shared_ptr<SystemManager> get();
	void runPreUpdates() const;
	void runupdates() const;
	void runPostUpdates() const;

	void addSystem(UpdateSteps step, std::unique_ptr<System> system);
};
} // namespace KEngine::myECS

#endif // !KENGINE_SYSTEM_HPP
