#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/fence_pool.hpp"
#include "vk1/core/semaphore_pool.hpp"

namespace vk1 {
class LogicalDevice;
class RenderTarget;
class FrameContext {
 public:
  NO_COPY_MOVE(FrameContext);

  FrameContext(const LogicalDevice& device, vk::Image swapchain_image);

 private:
  const LogicalDevice& logical_device_;
  FencePool fence_pool_;
  SemaphorePool semaphore_pool_;
  std::unique_ptr<RenderTarget> render_target_;
};
}  // namespace vk1
