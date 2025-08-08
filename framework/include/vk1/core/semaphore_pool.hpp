#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class SemaphorePool {
 public:
  NO_COPY_MOVE(SemaphorePool);

  SemaphorePool(const LogicalDevice& device);
  ~SemaphorePool();

 private:
  const LogicalDevice& logical_device_;
  std::vector<vk::Semaphore> semaphores_;
  std::vector<vk::Semaphore> released_semaphores_;
  uint32_t active_semaphore_count{0};
};
}  // namespace vk1
