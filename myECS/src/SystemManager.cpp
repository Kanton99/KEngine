#include <memory>
#include <myECS/SystemManager.hpp>
namespace KEngine::myECS {

void SystemManager::addSystem(UpdateSteps step, std::unique_ptr<System> system) {
	switch (step) {
	case UpdateSteps::UPDATE:
		this->m_updateSystems.push_back(std::move(system));
		break;
	case UpdateSteps::PREUPDATE:
		this->m_preUpdateSystems.push_back(std::move(system));
		break;
	case UpdateSteps::POSTUPDATE:
		this->m_postUpdateSystems.push_back(std::move(system));
		break;
	}
}

void SystemManager::runPreUpdates() const {
	for (const auto &system : m_preUpdateSystems) {
		system->start();
	}
}

void SystemManager::runupdates() const {
	for (const auto &system : m_updateSystems) {
		system->update();
	}
}

void SystemManager::runPostUpdates() const {
	for (const auto &system : m_postUpdateSystems) {
		system->update();
	}
}
} // namespace KEngine::myECS
