#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/instance.hpp"

namespace vk1 {
class PhysicalDevice {
 public:
  NO_COPY_MOVE(PhysicalDevice);

  explicit PhysicalDevice(const Instance& instance, VkPhysicalDevice vk_physical_device);

 public:
  inline const VkPhysicalDevice getVkPhysicalDevice() const {
    return vk_physical_device_;
  }
  inline const std::vector<const char*>& getEnabledExtensions() const {
    return enabled_device_extensions_;
  }
  inline const std::vector<VkQueueFamilyProperties>& getQueueFamilies() const {
    return queue_family_properties;
  }

  inline const VkPhysicalDeviceProperties& getProperties() const {
    return properties_;
  }

  inline const VkPhysicalDeviceFeatures& getFeatures() const {
    return features_;
  }

  bool supportExtensions(const OptionalExtensions& required_extensions);

 private:
  const Instance& instance_;
  VkPhysicalDevice vk_physical_device_;
  VkPhysicalDeviceFeatures features_;
  std::vector<VkExtensionProperties> device_extensions_;
  std::vector<const char*> enabled_device_extensions_;
  VkPhysicalDeviceProperties properties_;
  VkPhysicalDeviceMemoryProperties memory_properties;
  std::vector<VkQueueFamilyProperties> queue_family_properties;
  // surface details
  VkSurfaceCapabilitiesKHR capabilities_;
  std::vector<VkSurfaceFormatKHR> formats_;
  std::vector<VkPresentModeKHR> present_modes_;
};
}  // namespace vk1
