#include "myECS/Component.hpp"
#include <myECS/ComponentStorage.hpp>
#include <myECS/Entity.hpp>
#include <vector>

namespace KEngine::myECS {
template <class... Components> class Archetype {
  public: // Methods
	Signature getSignature() { return this->m_signature; }

  private: // Members
	Signature m_signature;
};
} // namespace KEngine::myECS
