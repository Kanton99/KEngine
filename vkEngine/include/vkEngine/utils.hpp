#ifndef VK_ENGINE_UTILS_HPP
#define VK_ENGINE_UTILS_HPP

#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>
namespace vkEngine {
static std::vector<char> readFile(const std::string &fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	std::vector<char> buffer(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

	file.close();

	return buffer;
}
} // namespace vkEngine
#endif // !VK_ENGINE_UTILS_HPP
