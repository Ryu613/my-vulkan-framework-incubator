#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Image;
class RenderTarget {
 public:
  NO_COPY_MOVE(RenderTarget);

  RenderTarget(const LogicalDevice& device);

  ~RenderTarget();

 private:
  const LogicalDevice& logical_device_;
  std::vector<Image> images_;
};
}  // namespace vk1
