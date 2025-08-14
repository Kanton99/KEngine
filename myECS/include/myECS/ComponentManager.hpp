#include "Entity.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace KEngine::myECS {
class ComponentsManagerBase {
  public:
	virtual ~ComponentsManagerBase() = default;
};

template <class C> class ComponentsManager : public ComponentsManagerBase {
	// Methods
  public:
	ComponentsManager(const ComponentsManager &) = delete;
	ComponentsManager(ComponentsManager &) = delete;
	ComponentsManager &operator=(const ComponentsManager &) = delete;
	ComponentsManager &operator=(ComponentsManager &) = delete;
	inline static std::shared_ptr<ComponentsManager> get() {
		static std::shared_ptr<ComponentsManager<C>> instance = std::make_shared<ComponentsManager>();
		return instance;
	}

	/**
	 *	Create a component of type C to entity
	 */
	inline void addComponent(EntityId entity) {
		this->m_components.push_back(new C());
		int size = this->m_components.size();
		this->m_entity_comp_reference[entity] = size;
		this->m_comp_entity_reference[size] = entity;
	}
	/**
	 *	Add a component of type C to entity
	 */
	inline void addComponent(EntityId entity, C *component) {
		this->m_components.push_back(component);
		int size = this->m_components.size();
		this->m_entity_comp_reference[entity] = size;
		this->m_comp_entity_reference[size] = entity;
	}

	/**
	 * Revome a component of type C from entity
	 */
	void removeComponent(EntityId entity);

	// Methods
  private:
	ComponentsManager();

	// Members
  private:
	std::unordered_map<EntityId, int> m_entity_comp_reference;
	std::unordered_map<int, EntityId> m_comp_entity_reference;
	std::vector<C> m_components;
};
} // namespace KEngine::myECS
