#ifndef VK_OBJ_LOADER
#define VK_OBJ_LOADER

#include "util_structs.hpp"
#include "vkEngine.hpp"
#include <optional>
#include <vector>
#include <filesystem>
namespace mvk{
std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadObj(vkEngine* engine, std::filesystem::path filePath);
}

#endif // !VK_OBJ_LOADER
