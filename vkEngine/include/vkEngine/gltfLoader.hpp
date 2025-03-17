#ifndef GLTF_LOADER
#define GLTF_LOADER
#include "util_structs.hpp"
#include "vkEngine.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace mvk{ 
std::optional<std::vector<std::shared_ptr<mvk::MeshAsset>>> loadGltfMesh(mvk::vkEngine* engine, std::filesystem::path filePath);
}
#endif
