#ifndef DESCRIPTOR_ALLOCATOR_GROWABLE
#define DESCRIPTOR_ALLOCATOR_GROWABLE

#include <vector>
#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace mvk{
  struct DescriptoAllocatorGrowable{
  public:
    struct PoolSizeRatio{
      vk::DescriptorType type;
      float ratio;
    };

    void init(vk::Device device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
    void clearPools(vk::Device device);
    void destroyPools(vk::Device device);

    vk::DescriptorSet allocate(vk::Device device, vk::DescriptorSetLayout layout, void* pNext = nullptr);

  private:
    vk::DescriptorPool getPool(vk::Device device);
    vk::DescriptorPool createPool(vk::Device device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

  private:
    std::vector<PoolSizeRatio> ratios;
    std::vector<vk::DescriptorPool> fullPools;
    std::vector<vk::DescriptorPool> readyPools;
    uint32_t setsPerPool;
  };
}

#endif
