#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>
#include <filesystem>
#include <iostream>

static std::vector<char> read_file(const std::string& filename){
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if(!file.is_open()){
      std::cout << std::filesystem::current_path() << "\n";
      auto error_message = std::strerror(errno);
      throw std::runtime_error(std::format("failed to open file: {}", error_message));
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

vk::ShaderModule create_shader_module(const std::vector<char> shader, const vk::Device device) {
    vk::ShaderModuleCreateInfo shader_mod_create_info{
        .codeSize = shader.size(),
        .pCode = reinterpret_cast<const uint32_t*>(shader.data())
    };

    vk::ShaderModule module;
    module = device.createShaderModule(shader_mod_create_info);
    return module;
}