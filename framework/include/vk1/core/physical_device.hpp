#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/instance.hpp"

namespace vk1 {
class PhysicalDevice final {
 public:
  NO_COPY_MOVE(PhysicalDevice);

  explicit PhysicalDevice(const Instance& instance, vk::PhysicalDevice vk_physical_device);

 public:
  inline const Instance& getInstance() const {
    return instance_;
  }
  inline const vk::PhysicalDevice getVkPhysicalDevice() const {
    return vk_physical_device_;
  }
  inline const std::vector<const char*>& getEnabledExtensions() const {
    return enabled_device_extensions_;
  }
  inline const std::vector<vk::QueueFamilyProperties>& getQueueFamilies() const {
    return queue_family_properties_;
  }

  inline const vk::PhysicalDeviceProperties& getProperties() const {
    return properties_;
  }

  inline const vk::PhysicalDeviceFeatures& getFeatures() const {
    return features_;
  }

  bool supportExtensions(const OptionalExtensions& required_extensions);

  void createLogicalDevice(std::set<uint32_t> queue_family_indices, bool is_debug);

  vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates,
                               vk::ImageTiling tiling,
                               vk::FormatFeatureFlags features) const;

 private:
  const Instance& instance_;
  vk::PhysicalDevice vk_physical_device_;
  vk::PhysicalDeviceFeatures features_;
  std::vector<vk::ExtensionProperties> device_extensions_;
  std::vector<const char*> enabled_device_extensions_;
  vk::PhysicalDeviceProperties properties_;
  vk::PhysicalDeviceMemoryProperties memory_properties_;
  std::vector<vk::QueueFamilyProperties> queue_family_properties_;
};
}  // namespace vk1
