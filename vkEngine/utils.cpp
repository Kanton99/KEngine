#include "utils.hpp"
#include <filesystem>
#include <iostream>
#include <cstdint>
#include <fstream>
#include <stdexcept>

vk::ShaderModule createShaderModule(const std::vector<char> shader,
                                    const vk::Device device) {
  vk::ShaderModuleCreateInfo shaderModCreateInfo;
  shaderModCreateInfo.setCodeSize(shader.size());
  shaderModCreateInfo.setPCode(
      reinterpret_cast<const uint32_t *>(shader.data()));
  vk::ShaderModule module;
  module = device.createShaderModule(shaderModCreateInfo);
  return module;
}

void transitionImage(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout src,
                     vk::ImageLayout dst) {
  vk::ImageMemoryBarrier2 imageBarrier{
    .srcStageMask = vk::PipelineStageFlagBits2::eAllCommands,
    .srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
    .dstStageMask = vk::PipelineStageFlagBits2::eAllCommands,
    .dstAccessMask =
        vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,

    .oldLayout = src,
    .newLayout = dst,
    .image = image,
  };

  vk::ImageAspectFlags aspectMask = (dst == vk::ImageLayout::eDepthAttachmentOptimal) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

  vk::ImageSubresourceRange subResourceRange{
    .aspectMask = aspectMask,
    .baseMipLevel = 0,
    .levelCount = vk::RemainingMipLevels,
    .baseArrayLayer = 0,
    .layerCount = vk::RemainingArrayLayers
  };
  imageBarrier.subresourceRange = subResourceRange;

  vk::DependencyInfo depInfo{
    .pImageMemoryBarriers = &imageBarrier
  };

  cmd.pipelineBarrier2(depInfo);
}

std::vector<char> readFile(const std::string &filename) {
  std::string currentPath = std::filesystem::current_path().generic_string();
  std::string absolutePath = currentPath + "/" + filename;
  std::ifstream file(absolutePath, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    auto error_message = std::strerror(errno);
    throw std::runtime_error(std::format("failed to open file: {} at {}",
                                         error_message, absolutePath));
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

