#ifndef ECS_COMPONENT_HPP
#define ECS_COMPONENT_HPP

#include <concepts>
#include <myECS/definitions.hpp>
namespace KEngine::myECS {

class IComponent { // base class to have common signature counter;
protected:
  static unsigned int signCounter;
};

template <typename T> class Component : public IComponent {
public:
  static const Signature signature;

private:
  static inline constexpr Signature getSignature() {
    return 1 << signCounter++;
  }
};

template <class C>
concept ComponentType = requires(C c) { std::derived_from<C, Component<C>>; };

template <typename T>
const Signature Component<T>::signature = Component<T>::getSignature();

} // namespace KEngine::myECS

#endif // !ECS_COMPONENT_HPP
