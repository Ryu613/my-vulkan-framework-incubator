#include "vk1/core/physical_device.hpp"

#include "vk1/common/util.hpp"

namespace vk1 {
PhysicalDevice::PhysicalDevice(const Instance& instance, VkPhysicalDevice vk_physical_device)
    : instance_(instance), vk_physical_device_(vk_physical_device) {
  // features
  vkGetPhysicalDeviceFeatures(vk_physical_device_, &features_);
  // properties
  vkGetPhysicalDeviceProperties(vk_physical_device_, &properties_);
  // memory properties
  vkGetPhysicalDeviceMemoryProperties(vk_physical_device_, &memory_properties);
  // queue family info
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device_, &queueFamilyCount, nullptr);
  queue_family_properties.resize(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(
      vk_physical_device_, &queueFamilyCount, queue_family_properties.data());
  // extensions
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(vk_physical_device_, nullptr, &extensionCount, nullptr);
  device_extensions_.resize(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      vk_physical_device_, nullptr, &extensionCount, device_extensions_.data());
}

bool PhysicalDevice::supportExtensions(const OptionalExtensions& required_extensions) {
  auto [supported, enabledNames] = util::isSupported<VkExtensionProperties>(
      device_extensions_, required_extensions, [](const VkExtensionProperties& p) {
        return p.extensionName;
      });
  if (supported) {
    enabled_device_extensions_.insert(
        enabled_device_extensions_.end(), enabledNames.begin(), enabledNames.end());
  }
  return supported;
}
}  // namespace vk1
