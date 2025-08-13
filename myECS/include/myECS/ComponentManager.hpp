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
	static std::shared_ptr<ComponentsManager> get();

	/**
	 *	Create a component of type C to entity
	 */
	void addComponent(EntityId entity);

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
