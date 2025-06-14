#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/swapchain.hpp"

namespace vk1 {
class RenderContext {
 public:
  NO_COPY_MOVE(RenderContext);

  explicit RenderContext(const LogicalDevice& logical_device);

 private:
  const LogicalDevice& logical_device_;
  std::unique_ptr<Swapchain> swapchain_;
};
}  // namespace vk1
