#include "vk1/core/logical_device.hpp"

namespace vk1 {
LogicalDevice::LogicalDevice(const PhysicalDevice& physical_device, VkSurfaceKHR surface)
    : physical_device_(physical_device), queue_family_info_(findQueueFamilyIndex(physical_device, surface)) {
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : queue_family_info_.getIndices()) {
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    queueCreateInfos.push_back(queueCreateInfo);
  }
  const auto& instance = getPhysicalDevice().getInstance();
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(instance.getEnabledlayers().size()),
      .ppEnabledLayerNames = instance.getEnabledlayers().data(),
      .enabledExtensionCount = static_cast<uint32_t>(physical_device.getEnabledExtensions().size()),
      .ppEnabledExtensionNames = physical_device.getEnabledExtensions().data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  if (vkCreateDevice(physical_device.getVkPhysicalDevice(), &createInfo, nullptr, &vk_device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  volkLoadDevice(vk_device_);
  // vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  // vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

LogicalDevice::~LogicalDevice() {}

QueueFamilyInfo findQueueFamilyIndex(const PhysicalDevice& physical_device, VkSurfaceKHR surface) {
  const auto& queueFamilyProperties = physical_device.getQueueFamilies();
  QueueFamilyInfo res{};
  uint32_t index = 0;
  for (const auto& queueFamily : queueFamilyProperties) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      res.graphics_queue_family_index = index;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        physical_device.getVkPhysicalDevice(), index, surface, &presentSupport);
    if (presentSupport) {
      res.present_queue_family_index = index;
    }
    if (res.graphics_queue_family_index.has_value() && res.present_queue_family_index.has_value()) {
      break;
    }
    index++;
  }
  return res;
}
}  // namespace vk1
