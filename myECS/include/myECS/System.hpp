#ifndef KENGINE_SYSTEM_HPP
#define KENGINE_SYSTEM_HPP

namespace KEngine::myECS {
class System {
public:
  virtual void update() const = 0;
  virtual void start() const = 0;
};
} // namespace KEngine

#endif // !KENGINE_SYSTEM_HPP
