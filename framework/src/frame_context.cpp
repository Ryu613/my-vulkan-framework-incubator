#include "vk1/rendering/frame_context.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/rendering/render_target.hpp"
#include "vk1/core/image.hpp"

namespace vk1 {
FrameContext::FrameContext(const LogicalDevice& device, std::vector<Image>&& render_images)
    : logical_device_(device), fence_pool_(logical_device_), semaphore_pool_(logical_device_) {
  // create buffer pool
  // create render target
  render_target_ = std::make_unique<RenderTarget>(logical_device_, std::move(render_images));
}
}  // namespace vk1
