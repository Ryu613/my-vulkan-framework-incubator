#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct QueueFamilyInfo {
  std::optional<uint32_t> graphics_queue_family_index;
  std::optional<uint32_t> present_queue_family_index;
  bool hasIndices(uint32_t index) const {
    if (index != graphics_queue_family_index.value() && index != present_queue_family_index.value()) {
      return false;
    }
    return true;
  }
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

class LogicalDevice final {
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

  inline const QueueFamilyInfo& getQueueFamilyInfo() const {
    return queue_family_info_;
  }

  void createCommandPoolAndBuffers(uint32_t queue_family_index, uint32_t command_buffer_count);

 private:
  const PhysicalDevice& physical_device_;
  VkDevice vk_device_;
  QueueFamilyInfo queue_family_info_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;

 private:
  QueueFamilyInfo findQueueFamilyIndex(const PhysicalDevice& physical_device, VkSurfaceKHR surface);
};

}  // namespace vk1
