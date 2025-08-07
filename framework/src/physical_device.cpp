#include "vk1/core/physical_device.hpp"

#include "vk1/common/util.hpp"

namespace vk1 {
PhysicalDevice::PhysicalDevice(const Instance& instance, vk::PhysicalDevice vk_physical_device)
    : instance_(instance), vk_physical_device_(vk_physical_device) {
  // features
  features_ = vk_physical_device_.getFeatures();
  // properties
  properties_ = vk_physical_device_.getProperties();
  // memory properties
  memory_properties_ = vk_physical_device_.getMemoryProperties();
  // queue family info
  queue_family_properties_ = vk_physical_device_.getQueueFamilyProperties();
  // extensions
  device_extensions_ = vk_physical_device_.enumerateDeviceExtensionProperties();
}

bool PhysicalDevice::supportExtensions(const OptionalExtensions& required_extensions) {
  auto [supported, enabledNames] = util::isSupported<vk::ExtensionProperties>(
      device_extensions_, required_extensions, [](const vk::ExtensionProperties& p) {
        return p.extensionName;
      });
  if (supported) {
    enabled_device_extensions_.insert(
        enabled_device_extensions_.end(), enabledNames.begin(), enabledNames.end());
  }
  return supported;
}
}  // namespace vk1
