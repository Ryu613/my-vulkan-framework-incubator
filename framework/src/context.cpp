#define VOLK_IMPLEMENTATION
#include "vk1/core/context.hpp"

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  initVulkan();
}

void Context::initVulkan() {
  createInstance();
  createSurface();
  const auto& suitable_physical_device =
      instance_->chooseSuitablePhysicalDevice(surface_, config_.required_device_extensions);
  // suitable_physical_device.createLogicalDevice();
  createSwapchain();
  createImageViews();
  createRenderPass();
  creategraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

void Context::createInstance() {
  if (volkInitialize()) {
    throw std::runtime_error("failed to initialize volk!");
  }
  instance_ = std::make_unique<Instance>(
      config_.app_name, config_.required_layers, config_.required_instance_extensions, config_.is_debug);

  volkLoadInstance(instance_->getVkInstance());
}
void Context::createSurface() {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  VkWin32SurfaceCreateInfoKHR createInfo{.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                         .hinstance = GetModuleHandle(NULL),
                                         .hwnd = (HWND)config_.window};
  if (vkCreateWin32SurfaceKHR(instance_->getVkInstance(), &createInfo, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create surface!");
  }
#else
  throw std::runtime_error("cannot create surface!");
#endif
}
void Context::pickPhysicalDevice() {
  const auto& suitable_physical_device =
      instance_->chooseSuitablePhysicalDevice(surface_, config_.required_device_extensions);
}
void Context::createLogicalDevice(const PhysicalDevice& physical_device) {
  populateQueueFamilyIndex(physical_device);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {graphics_queue_family_index_.value(),
                                            present_queue_family_index_.value()};
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    queueCreateInfos.push_back(queueCreateInfo);
  }
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      // modern vulkan implementations don't need this, just for compatible for old implementations
      .enabledExtensionCount = static_cast<uint32_t>(physical_device.getEnabledExtensions().size()),
      .ppEnabledExtensionNames = physical_device.getEnabledExtensions().data(),
      .pEnabledFeatures = &deviceFeatures,
  };
  if (config_.is_debug) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(instance_->getEnabledlayers().size());
    createInfo.ppEnabledLayerNames = instance_->getEnabledlayers().data();
  } else {
    createInfo.enabledLayerCount = 0;
  }
  // if (vkCreateDevice(physical_device.getVkPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
  //   throw std::runtime_error("failed to create logical device!");
  // }
  // vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  // vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}
void Context::createSwapchain() {}
void Context::createImageViews() {}
void Context::createRenderPass() {}
void Context::creategraphicsPipeline() {}
void Context::createFramebuffers() {}
void Context::createCommandPool() {}
void Context::createCommandBuffers() {}
void Context::createSyncObjects() {}

void Context::populateQueueFamilyIndex(const PhysicalDevice& physical_device) {
  const auto& queueFamilyProperties = physical_device.getQueueFamilies();
  uint32_t index = 0;
  for (const auto& queueFamily : queueFamilyProperties) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_queue_family_index_ = index;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        physical_device.getVkPhysicalDevice(), index, surface_, &presentSupport);
    if (presentSupport) {
      present_queue_family_index_ = index;
    }
    if (graphics_queue_family_index_.has_value() && present_queue_family_index_.has_value()) {
      break;
    }
    index++;
  }
}

}  // namespace vk1
