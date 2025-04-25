#ifndef KENGINE_ENTITY_HPP
#define KENGINE_ENTITY_HPP
#include <string>
// #include <stduuid/uuids.h> TODO fix uuid lib issue

namespace KEngine::myECS {
class Entity {
public: // Members
  std::string name;
  // uuids::uuid id;
  uint32_t id;
};
} // namespace KEngine::myECS

#endif
