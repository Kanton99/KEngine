#include "myECS/Entity.hpp"
#include <memory>
#include <myECS/ComponentManager.hpp>
namespace KEngine::myECS {
//----------------------------------------------------------------------------------------------------------
template <class C> std::shared_ptr<ComponentsManager<C>> ComponentsManager<C>::get() {
	static std::shared_ptr<ComponentsManager<C>> instance = std::make_shared<ComponentsManager>();
	return instance;
}

//----------------------------------------------------------------------------------------------------------
template <class C> void ComponentsManager<C>::addComponent(EntityId entity) {
	this->m_components.push_back(new C());
	int size = this->m_components.size();
	this->m_entity_comp_reference[entity] = size;
	this->m_comp_entity_reference[size] = entity;
}

//----------------------------------------------------------------------------------------------------------
template <class C> void ComponentsManager<C>::removeComponent(EntityId entity) { int pos = this->m_entity_comp_reference[entity]; }
} // namespace KEngine::myECS
