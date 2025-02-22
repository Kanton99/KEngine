#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>
#include <filesystem>
#include <iostream>

static std::vector<char> readFile(const std::string& filename){
  std::string currentPath = std::filesystem::current_path();
  std::string absolutePath = currentPath+"/"+filename;
  std::ifstream file(absolutePath, std::ios::ate | std::ios::binary);

  if(!file.is_open()){
      auto error_message = std::strerror(errno);
      throw std::runtime_error(std::format("failed to open file: {} at {}", error_message, absolutePath));
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

vk::ShaderModule createShaderModule(const std::vector<char> shader, const vk::Device device) {
    vk::ShaderModuleCreateInfo shaderModCreateInfo;
    shaderModCreateInfo.setCodeSize(shader.size());
    shaderModCreateInfo.setPCode(reinterpret_cast<const uint32_t*>(shader.data()));  
    vk::ShaderModule module;
    module = device.createShaderModule(shaderModCreateInfo);
    return module;
}
