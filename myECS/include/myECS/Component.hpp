#ifndef KENGINE_COMPONENT_HPP
#define KENGINE_COMPONENT_HPP

#include "Entity.hpp"

namespace KEngine::myECS {
class BaseComponent {
  protected:
	static inline Signature next_signature = 1;

	static Signature get_next_signature() {
		Signature current = next_signature;
		next_signature <<= 1;
		return current;
	}
};

template <typename Derived> class Component : private BaseComponent {
  public:
	// Component type signature
	static inline const Signature signature = Component<Derived>::get_next_signature();
};

} // namespace KEngine::myECS

#endif // !KENGINE_COMPONENT_HPP
