#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/instance.hpp"

namespace vk1 {
class PhysicalDevice final {
 public:
  NO_COPY_MOVE(PhysicalDevice);

  explicit PhysicalDevice(const Instance& instance, VkPhysicalDevice vk_physical_device);

 public:
  inline const Instance& getInstance() const {
    return instance_;
  }
  inline const VkPhysicalDevice getVkPhysicalDevice() const {
    return vk_physical_device_;
  }
  inline const std::vector<const char*>& getEnabledExtensions() const {
    return enabled_device_extensions_;
  }
  inline const std::vector<VkQueueFamilyProperties>& getQueueFamilies() const {
    return queue_family_properties_;
  }

  inline const VkPhysicalDeviceProperties& getProperties() const {
    return properties_;
  }

  inline const VkPhysicalDeviceFeatures& getFeatures() const {
    return features_;
  }

  bool supportExtensions(const OptionalExtensions& required_extensions);

  void createLogicalDevice(std::set<uint32_t> queue_family_indices, bool is_debug);

 private:
  const Instance& instance_;
  VkPhysicalDevice vk_physical_device_ = VK_NULL_HANDLE;
  VkPhysicalDeviceFeatures features_{};
  std::vector<VkExtensionProperties> device_extensions_;
  std::vector<const char*> enabled_device_extensions_;
  VkPhysicalDeviceProperties properties_{};
  VkPhysicalDeviceMemoryProperties memory_properties_{};
  std::vector<VkQueueFamilyProperties> queue_family_properties_;
};
}  // namespace vk1
