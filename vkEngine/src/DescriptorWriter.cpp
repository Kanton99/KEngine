#include "vkEngine/DescriptorWriter.hpp"

void mvk::DescriptorWriter::writeBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type){
  vk::DescriptorBufferInfo& info = this->bufferInfos.emplace_back(vk::DescriptorBufferInfo{ 
    .buffer = buffer, 
    .offset = offset,
    .range = size
  });

  vk::WriteDescriptorSet write;
  write.setDstBinding(binding);
  write.setDescriptorCount(1);
  write.setDescriptorType(type);
  write.setBufferInfo(info);

  writes.push_back(write);
}
void mvk::DescriptorWriter::writeImage(int binding,vk::ImageView image, vk::Sampler sampler,  vk::ImageLayout layout, vk::DescriptorType type)
{
  vk::DescriptorImageInfo& info = imageInfos.emplace_back(vk::DescriptorImageInfo{
    .sampler = sampler,
    .imageView =image,
    .imageLayout = layout
  });

  vk::WriteDescriptorSet write;

  write.dstBinding = binding;
  write.descriptorCount = 1;
  write.descriptorType = type;
  write.pImageInfo = &info;

  writes.push_back(write);
}

void mvk::DescriptorWriter::clear(){
  this->bufferInfos.clear();
  this->imageInfos.clear();
  this->writes.clear();
}

void mvk::DescriptorWriter::updateSet(vk::Device device, vk::DescriptorSet set){
  for(auto& write : writes){
    write.setDstSet(set);
  }

  device.updateDescriptorSets(writes, nullptr);
}
