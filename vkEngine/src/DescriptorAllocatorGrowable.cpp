#include <algorithm>
#include <vector>
#include <vkEngine/DescriptorAllocator.hpp>
#include <vulkan/vulkan.hpp>

vk::DescriptorPool mvk::DescriptoAllocatorGrowable::getPool(vk::Device device){
  vk::DescriptorPool newPool;
  if(this->readyPools.size() != 0){
    newPool = readyPools.back();
    readyPools.pop_back();
  } else {
    newPool = this->createPool(device, this->setsPerPool, this->ratios);

    setsPerPool = std::clamp(setsPerPool * 1.5f, 0.f, 4096.f);
  }
  return newPool;
}

vk::DescriptorPool mvk::DescriptoAllocatorGrowable::createPool(vk::Device device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios) {
  std::vector<vk::DescriptorPoolSize> poolSizes;
  for(PoolSizeRatio ratio : poolRatios){
    poolSizes.push_back(vk::DescriptorPoolSize{
      .type = ratio.type,
      .descriptorCount = uint32_t(ratio.ratio*setCount),
    });
  }

  vk::DescriptorPoolCreateInfo poolInfo{
    .maxSets = setCount,
  };
  poolInfo.setPoolSizes(poolSizes);

  vk::DescriptorPool newPool = device.createDescriptorPool(poolInfo);
  return newPool;
}

void mvk::DescriptoAllocatorGrowable::init(vk::Device device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios){
  this->ratios.clear();

  for(auto r : poolRatios) this->ratios.push_back(r);

  auto newPool = this->createPool(device, initialSets, poolRatios);

  setsPerPool = initialSets * 1.5f;
  readyPools.push_back(newPool);
}
    
void mvk::DescriptoAllocatorGrowable::clearPools(vk::Device device){
  for(auto p : this->readyPools) device.resetDescriptorPool(p);
  for(auto p : this->fullPools) {
    device.resetDescriptorPool(p);
  readyPools.push_back(p);
  }
  fullPools.clear();
}

void mvk::DescriptoAllocatorGrowable::destroyPools(vk::Device device){
  for(auto p : this->readyPools) device.destroyDescriptorPool(p);
  this->readyPools.clear();
  for(auto p : this->fullPools) device.destroyDescriptorPool(p);
  this->fullPools.clear();
}

vk::DescriptorSet mvk::DescriptoAllocatorGrowable::allocate(vk::Device device, vk::DescriptorSetLayout layout, void* pNext){
  auto poolToUse = this->getPool(device);

  vk::DescriptorSetAllocateInfo info{
    .pNext = pNext,
    .descriptorPool = poolToUse,
    .descriptorSetCount = 1,
    .pSetLayouts = &layout
  };

  vk::DescriptorSet ds;
  try{
  auto result = device.allocateDescriptorSets(info);
  }catch(vk::SystemError error){
    if(error.code() == vk::Result::eErrorOutOfPoolMemory || error.code() == vk::Result::eErrorFragmentedPool) {
      this->fullPools.push_back(poolToUse);
      poolToUse = this->getPool(device);
      info.setDescriptorPool(poolToUse);
      ds = device.allocateDescriptorSets(info).front();
    }
  }
  readyPools.push_back(poolToUse);
  return ds;
}
