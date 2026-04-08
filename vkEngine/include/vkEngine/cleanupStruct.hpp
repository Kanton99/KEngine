#ifndef CLEANUP_QUEUE_HPP
#define CLEANUP_QUEUE_HPP

#include <deque>
#include <functional>
namespace vkEngine {
struct CleanupQueue {
	std::deque<std::function<void()>> cleanupStack;

	void pushFunction(std::function<void()> &&function) { cleanupStack.push_back(function); }

	void flush() {
		for (auto it = cleanupStack.rbegin(); it != cleanupStack.rend(); it++) {
			(*it)();
		}

		cleanupStack.clear();
	}
};
} // namespace vkEngine

#endif // !CLEANUP_QUEUE_HPP
