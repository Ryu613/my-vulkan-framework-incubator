#include "vk1/rendering/frame_context.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/rendering/render_target.hpp"

namespace vk1 {
FrameContext::FrameContext(const LogicalDevice& device, vk::Image swapchain_image)
    : logical_device_(device), fence_pool_(logical_device_), semaphore_pool_(logical_device_) {
  // create buffer pool
  // create color image and depth image
  std::vector<Image> images;
  vk::Format depthFormat = logical_device_.getPhysicalDevice().findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment);
  // create render target
  render_target_ = std::make_unique<RenderTarget>();
}
}  // namespace vk1
