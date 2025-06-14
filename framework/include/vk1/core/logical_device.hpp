#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct QueueFamilyInfo {
  std::optional<uint32_t> graphics_queue_family_index;
  std::optional<uint32_t> present_queue_family_index;
  std::set<uint32_t> getIndices() const {
    if (isComplete()) {
      return {graphics_queue_family_index.value(), present_queue_family_index.value()};
    }
    return {};
  }
  bool isComplete() const {
    return graphics_queue_family_index.has_value() && present_queue_family_index.has_value();
  }
};
class LogicalDevice {
 public:
  NO_COPY_MOVE(LogicalDevice);

  explicit LogicalDevice(const PhysicalDevice& physical_device, VkSurfaceKHR surface);

  ~LogicalDevice();

 public:
  inline const PhysicalDevice& getPhysicalDevice() const {
    return physical_device_;
  }

  inline const VkDevice& getVkDevice() const {
    return vk_device_;
  }

 private:
  const PhysicalDevice& physical_device_;
  VkDevice vk_device_;
  QueueFamilyInfo queue_family_info_;
};

QueueFamilyInfo findQueueFamilyIndex(const PhysicalDevice& physical_device, VkSurfaceKHR surface);
}  // namespace vk1
