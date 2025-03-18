#ifndef MVK_DESCRIPTOR_WRITER
#define MVK_DESCRIPTOR_WRITER

#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <deque>

namespace mvk{
struct DescriptorWriter{
  std::deque<vk::DescriptorImageInfo> imageInfos;
  std::deque<vk::DescriptorBufferInfo> bufferInfos;
  std::vector<vk::WriteDescriptorSet> writes;

  void writeImage(int binding, vk::ImageView view, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type);
  void writeBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type);

  void clear();
  void updateSet(vk::Device device, vk::DescriptorSet set);
};
}

#endif // !MVK_DESCRIPTOR_WRITE"

