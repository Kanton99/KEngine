#ifndef KENGINE_COMPONENT_HPP
#define KENGINE_COMPONENT_HPP

#include "Entity.hpp"
#include <memory>

namespace KEngine::myECS {
class Component {
	std::shared_ptr<Entity> entity;
};
} // namespace KEngine::myECS

#endif // !KENGINE_COMPONENT_HPP
