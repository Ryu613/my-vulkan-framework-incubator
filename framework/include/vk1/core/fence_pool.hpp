#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class FencePool {
 public:
  NO_COPY_MOVE(FencePool);

  FencePool(const LogicalDevice& device);

  ~FencePool();

  vk::Fence acquireFence();

  vk::Result reset();

  vk::Result wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) const;

 private:
  const LogicalDevice& logical_device_;
  std::vector<vk::Fence> fences_;
  uint32_t active_fence_count{0};
};
}  // namespace vk1
