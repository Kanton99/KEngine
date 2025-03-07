#include "utils.hpp"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

vk::ShaderModule mvk::utils::createShaderModule(const std::vector<char> shader,
                                                const vk::Device device) {
  vk::ShaderModuleCreateInfo shaderModCreateInfo;
  shaderModCreateInfo.setCodeSize(shader.size());
  shaderModCreateInfo.setPCode(
      reinterpret_cast<const uint32_t *>(shader.data()));
  vk::ShaderModule module;
  module = device.createShaderModule(shaderModCreateInfo);
  return module;
}

void mvk::utils::transitionImage(vk::CommandBuffer cmd, vk::Image image,
                                 vk::ImageLayout oldLayout,
                                 vk::ImageLayout newLayout) {
  vk::ImageMemoryBarrier2 imageBarrier{
      .srcStageMask = vk::PipelineStageFlagBits2::eAllCommands,
      .srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
      .dstStageMask = vk::PipelineStageFlagBits2::eAllCommands,
      .dstAccessMask =
          vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,

      .oldLayout = oldLayout,
      .newLayout = newLayout,
      .image = image,
  };

  vk::ImageAspectFlags aspectMask =
      (newLayout == vk::ImageLayout::eDepthAttachmentOptimal)
          ? vk::ImageAspectFlagBits::eDepth
          : vk::ImageAspectFlagBits::eColor;

  imageBarrier.subresourceRange = {.aspectMask = aspectMask,
                                   .baseMipLevel = 0,
                                   .levelCount = vk::RemainingMipLevels,
                                   .baseArrayLayer = 0,
                                   .layerCount = vk::RemainingArrayLayers};

  std::array<vk::ImageMemoryBarrier2, 1> memoryBarriers = {imageBarrier};
  vk::DependencyInfoKHR depInfo{
      .imageMemoryBarrierCount = static_cast<uint32_t>(memoryBarriers.size()),
      .pImageMemoryBarriers = memoryBarriers.data(),
  };

  cmd.pipelineBarrier2(depInfo);
}

std::vector<char> mvk::utils::readFile(const std::string &filename) {
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

void mvk::utils::copyImageToImage(vk::CommandBuffer cmd, vk::Image source,
                                  vk::Image destination, vk::Extent2D srcSize,
                                  vk::Extent2D dstSize) {
  vk::ImageBlit2 blitRegion{
      .srcSubresource =
          {
              .aspectMask = vk::ImageAspectFlagBits::eColor,
              .layerCount = 1,
          },
      .dstSubresource = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
          .layerCount = 1,
      }};

  blitRegion.srcOffsets[1].x = srcSize.width;
  blitRegion.srcOffsets[1].y = srcSize.height;
  blitRegion.srcOffsets[1].z = 1;

  blitRegion.dstOffsets[1].x = dstSize.width;
  blitRegion.dstOffsets[1].y = dstSize.height;
  blitRegion.dstOffsets[1].z = 1;

  vk::BlitImageInfo2 blitInfo{
      .srcImage = source,
      .srcImageLayout = vk::ImageLayout::eTransferSrcOptimal,
      .dstImage = destination,
      .dstImageLayout = vk::ImageLayout::eTransferDstOptimal,
      .regionCount = 1,
      .pRegions = &blitRegion,
      .filter = vk::Filter::eLinear,
  };

  cmd.blitImage2(blitInfo);
}
