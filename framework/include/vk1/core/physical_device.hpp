#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/instance.hpp"

namespace vk1 {
class PhysicalDevice {
 public:
  NO_COPY_MOVE(PhysicalDevice);

 private:
  Instance& instance_;
  VkPhysicalDevice vk_physical_device_;
}
}  // namespace vk1
