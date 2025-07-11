#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Sampler final {
 public:
  NO_COPY_MOVE(Sampler);

  explicit Sampler(const LogicalDevice& device);

  ~Sampler();

 private:
  const LogicalDevice& logical_device_;
  VkSampler vk_sampler_{VK_NULL_HANDLE};
};
}  // namespace vk1
