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

 private:
  const LogicalDevice& logical_device_;
  std::vector<vk::Fence> fences_;
};
}  // namespace vk1
