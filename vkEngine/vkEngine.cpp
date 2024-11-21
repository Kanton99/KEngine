#include "vkEngine.h"
#include <SDL_video.h>
#include <SDL_vulkan.h>
#include <VkBootstrap.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_hpp_macros.hpp>
#include <vulkan/vulkan_structs.hpp>

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
#ifndef DYNAMIC_LOADER_LOADED
#define DYNAMIC_LOADER_LOADED
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif
#endif

mvk::vkEngine *mvk::vkEngine::_engine = nullptr;

mvk::vkEngine *mvk::vkEngine::get(SDL_Window *window) {
  if (!_engine) {
    _engine = new mvk::vkEngine(window);
  }
  return _engine;
}

void mvk::vkEngine::init() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init();
#endif // VULKAN_HPP_DISPATCH_LOADER_DYNAMIC

  this->_init_instance();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_instance);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

  VkSurfaceKHR surface;
  if (SDL_Vulkan_CreateSurface(this->_window,
                               static_cast<VkInstance>(this->_instance),
                               &surface) == SDL_FALSE) {
    std::cerr << "Error generating surface\n";
  }
  this->_surface = vk::SurfaceKHR(surface);

  this->_init_devices();

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(this->_device);
#endif // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

  this->_init_command_pool(this->graphics_queue_index);
  this->_allocate_command_buffer(this->_graphics_command_buffer);

  int width, height;
  SDL_GetWindowSize(this->_window, &width, &height);

  this->_init_swapchain(width, height);
}

void mvk::vkEngine::cleanup() { this->stack.flush(); }

mvk::vkEngine::vkEngine(SDL_Window *window) : _window(window) {}

void mvk::vkEngine::_allocate_command_buffer(vk::CommandBuffer buffer) {
  vk::CommandBufferAllocateInfo buffer_allocation_info{
      .commandPool = this->_command_pool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1};
  buffer = this->_device.allocateCommandBuffers(buffer_allocation_info).front();
}

void mvk::vkEngine::_record_command_buffer() {}

void mvk::vkEngine::_init_instance() {
  vk::ApplicationInfo app_info{.pApplicationName = "KEngine",
                               .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
                               .pEngineName = "mvk::vkEngine",
                               .engineVersion = VK_MAKE_VERSION(0, 0, 1),
                               .apiVersion = VK_API_VERSION_1_3};

  // Instance Extensions
  std::vector<const char *> instance_extension_names;
  unsigned int extCount;
  SDL_Vulkan_GetInstanceExtensions(this->_window, &extCount, nullptr);
  instance_extension_names.resize(extCount);

  SDL_Vulkan_GetInstanceExtensions(this->_window, &extCount,
                                   instance_extension_names.data());

  instance_extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  // Instance Layers
  std::vector<const char *> instance_layers = {"VK_LAYER_KHRONOS_validation"};

  vk::InstanceCreateInfo instance_info{.flags = {},
                                       .pApplicationInfo = &app_info};
  instance_info.setPEnabledExtensionNames(instance_extension_names);
  instance_info.setPEnabledLayerNames(instance_layers);
  this->_instance = vk::createInstance(instance_info);

  this->stack.push_function([=, this]() { this->_instance.destroy(); });

  this->stack.push_function(
      [=, this]() { this->_instance.destroySurfaceKHR(this->_surface); });
}
void mvk::vkEngine::_init_devices() {
  this->_phys_device = this->_instance.enumeratePhysicalDevices().front();

  std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      this->_phys_device.getQueueFamilyProperties();

  auto property_iterator =
      std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                   [](vk::QueueFamilyProperties const &qfp) {
                     return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
                   });
  size_t graphics_queue_family_index = static_cast<size_t>(
      std::distance(queueFamilyProperties.begin(), property_iterator));
  assert(graphics_queue_family_index < queueFamilyProperties.size());

  // determine a queueFamilyIndex that suports present
  // first check if the graphicsQueueFamiliyIndex is good enough
  size_t present_queue_family_index =
      this->_phys_device.getSurfaceSupportKHR(
          static_cast<uint32_t>(graphics_queue_family_index), this->_surface)
          ? graphics_queue_family_index
          : queueFamilyProperties.size();
  if (present_queue_family_index == queueFamilyProperties.size()) {
    // the graphicsQueueFamilyIndex doesn't support present -> look for an other
    // family index that supports both graphics and present
    for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
      if ((queueFamilyProperties[i].queueFlags &
           vk::QueueFlagBits::eGraphics) &&
          this->_phys_device.getSurfaceSupportKHR(static_cast<uint32_t>(i),
                                                  this->_surface)) {
        graphics_queue_family_index = static_cast<uint32_t>(i);
        present_queue_family_index = i;
        break;
      }
    }
    if (present_queue_family_index == queueFamilyProperties.size()) {
      // there's nothing like a single family index that supports both graphics
      // and present -> look for an other family index that supports present
      for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (this->_phys_device.getSurfaceSupportKHR(static_cast<uint32_t>(i),
                                                    this->_surface)) {
          present_queue_family_index = i;
          break;
        }
      }
    }
  }
  if ((graphics_queue_family_index == queueFamilyProperties.size()) ||
      (present_queue_family_index == queueFamilyProperties.size())) {
    throw std::runtime_error(
        "Could not find a queue for graphics or present -> terminating");
  }

  this->present_queue_index = present_queue_family_index;
  this->graphics_queue_index = graphics_queue_family_index;

  float queue_priotity = 0.f;
  vk::DeviceQueueCreateInfo device_queue_create_info{
      .flags = {},
      .queueFamilyIndex = static_cast<uint32_t>(graphics_queue_family_index),
      .queueCount = 1,
      .pQueuePriorities = &queue_priotity,
  };

  std::vector<const char *> device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::DeviceCreateInfo device_create_info{
      .flags = vk::DeviceCreateFlags(),
  };

  device_create_info.setQueueCreateInfos({device_queue_create_info});

  device_create_info.setPEnabledExtensionNames(device_extensions);

  this->_device = this->_phys_device.createDevice(device_create_info);

  this->stack.push_function([=, this]() { this->_device.destroy(); });
}

#ifndef NDEBUG
void mvk::vkEngine::_init_debug_utils() {
  std::vector<vk::ExtensionProperties> props =
      vk::enumerateInstanceExtensionProperties();

  auto propertyIterator = std::find_if(
      props.begin(), props.end(), [](vk::ExtensionProperties const &ep) {
        return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
      });
  if (propertyIterator == props.end()) {
    std::cout << "Something went very wrong, cannot find "
              << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << " extension" << std::endl;
    exit(1);
  }

  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo{
      .flags = {},
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
      .pfnUserCallback = nullptr};

  this->_debug_messanger = this->_instance.createDebugUtilsMessengerEXT(
      debugUtilsMessengerInfo, nullptr);

  this->stack.push_function([=, this]() {
    this->_instance.destroyDebugUtilsMessengerEXT(this->_debug_messanger);
  });
}
#endif // CREATE DEBUG UTILS MESSENGER

void mvk::vkEngine::_init_command_pool(int queue_family_index) {
  vk::CommandPoolCreateInfo create_command_pool_info{
      .flags = {vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
      .queueFamilyIndex = static_cast<uint32_t>(queue_family_index)};
  auto commandPool = this->_device.createCommandPool(create_command_pool_info);
  this->_command_pool = commandPool;

  this->stack.push_function(
      [=, this]() { this->_device.destroyCommandPool(commandPool); });
}

void mvk::vkEngine::_init_swapchain(uint32_t width, uint32_t height) {

  // Get Format
  auto formats = this->_phys_device.getSurfaceFormatsKHR(this->_surface);
  assert(!formats.empty());
  vk::Format format = (formats[0].format == vk::Format::eUndefined)
                          ? vk::Format::eB8G8R8Unorm
                          : formats[0].format;

  // Get Image Extent
  auto surfaceCapabilities =
      this->_phys_device.getSurfaceCapabilitiesKHR(this->_surface);
  vk::Extent2D swapChainExtent;
  if (surfaceCapabilities.currentExtent.width ==
      (std::numeric_limits<uint32_t>::max)()) {
    swapChainExtent.width =
        std::clamp(width, surfaceCapabilities.minImageExtent.width,
                   surfaceCapabilities.maxImageExtent.width);
    swapChainExtent.height =
        std::clamp(height, surfaceCapabilities.minImageExtent.height,
                   surfaceCapabilities.maxImageExtent.height);
  } else {
    swapChainExtent = surfaceCapabilities.currentExtent;
  }

  // Pre Transform
  vk::SurfaceTransformFlagBitsKHR preTransform =
      (surfaceCapabilities.supportedTransforms &
       vk::SurfaceTransformFlagBitsKHR::eIdentity)
          ? vk::SurfaceTransformFlagBitsKHR::eIdentity
          : surfaceCapabilities.currentTransform;

  // Composite Alpha
  auto composite_alpha = (surfaceCapabilities.supportedCompositeAlpha &
                          vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
                             ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
                         : (surfaceCapabilities.supportedCompositeAlpha &
                            vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
                             ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
                         : (surfaceCapabilities.supportedCompositeAlpha &
                            vk::CompositeAlphaFlagBitsKHR::eInherit)
                             ? vk::CompositeAlphaFlagBitsKHR::eInherit
                             : vk::CompositeAlphaFlagBitsKHR::eOpaque;

  vk::SwapchainCreateInfoKHR swapchain_create_info{
      .flags = {},
      .surface = this->_surface,
      .minImageCount = surfaceCapabilities.minImageCount,
      .imageFormat = format,
      .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
      .imageExtent = swapChainExtent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .preTransform = preTransform,
      .compositeAlpha = composite_alpha,
      .presentMode = vk::PresentModeKHR::eFifo,
      .clipped = false};

  if (this->graphics_queue_index != this->present_queue_index) {
    uint32_t queue_family_indeces[2] = {
        static_cast<uint32_t>(this->graphics_queue_index),
        static_cast<uint32_t>(this->present_queue_index)};

    swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indeces;
  }

  this->graphic_swapchain.swapchain =
      this->_device.createSwapchainKHR(swapchain_create_info);

  this->stack.push_function([=, this]() {
    this->_device.destroySwapchainKHR(this->graphic_swapchain.swapchain);
  });

  this->graphic_swapchain.images =
      this->_device.getSwapchainImagesKHR(this->graphic_swapchain.swapchain);

  this->graphic_swapchain.imageViews.reserve(
      this->graphic_swapchain.images.size());

  vk::ImageViewCreateInfo image_view_info{
      .viewType = vk::ImageViewType::e2D,
      .format = format,
      .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
  for (auto image : this->graphic_swapchain.images) {
    image_view_info.image = image;
    this->graphic_swapchain.imageViews.push_back(
        this->_device.createImageView(image_view_info));
  }

  this->stack.push_function([=, this]() {
    for (auto imageView : this->graphic_swapchain.imageViews) {
      this->_device.destroyImageView(imageView);
    }
  });
}
