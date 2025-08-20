#include "Entity.hpp"
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

namespace KEngine::myECS {
class ComponentsStorageBase {
  public:
	virtual ~ComponentsStorageBase() = default;
};

template <class C> class ComponentsStorage : public ComponentsStorageBase {
	// Methods
  public:
	ComponentsStorage(const ComponentsStorage &) = delete;
	ComponentsStorage(ComponentsStorage &) = delete;
	ComponentsStorage &operator=(const ComponentsStorage &) = delete;
	ComponentsStorage &operator=(ComponentsStorage &) = delete;
	inline static std::shared_ptr<ComponentsStorage> get() {
		static std::shared_ptr<ComponentsStorage<C>> instance = std::make_shared<ComponentsStorage>();
		return instance;
	}

	/**
	 *	Create a component of type C to entity
	 */
	inline void addComponent(EntityId entity) {
		this->m_components.emplace_back(new C());
		int size = this->m_components.size();
		this->m_entity_comp_reference[entity] = size;
		this->m_comp_entity_reference[size] = entity;
	}

	/**
	 *	Add a component of type C to entity
	 */
	inline void addComponent(EntityId entity, C &component) {
		this->m_components.emplace_back(component);
		int size = this->m_components.size();
		this->m_entity_comp_reference[entity] = size;
		this->m_comp_entity_reference[size] = entity;
	}

	/**
	 * Revome a component of type C from entity
	 */
	inline void removeComponent(EntityId entity) {
		// remove component at it's position
		int pos = this->m_entity_comp_reference[entity];
		int endPos = this->m_components.size() - 1;
		EntityId endEntity = this->m_comp_entity_reference[endPos];
		std::iter_swap(this->m_components.begin() + pos, this->m_components.end() - 1);
		this->m_entity_comp_reference[endEntity] = pos;
		this->m_comp_entity_reference[pos] = endEntity;

		this->m_entity_comp_reference.erase(entity);
		this->m_comp_entity_reference.erase(endPos);

		this->m_components.erase(this->m_components.end() - 1);
	}

	// Methods
  private:
	ComponentsStorage();

	// Members
  private:
	std::unordered_map<EntityId, int> m_entity_comp_reference;
	std::unordered_map<int, EntityId> m_comp_entity_reference;
	std::vector<C> m_components;
};
} // namespace KEngine::myECS
