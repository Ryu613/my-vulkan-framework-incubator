#include "vk1/rendering/render_context.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/swapchain.hpp"
#include "vk1/platform/window.hpp"
#include "vk1/rendering/frame_context.hpp"
#include "vk1/core/image.hpp"

namespace vk1 {
RenderContext::RenderContext(const LogicalDevice& device, const Window& window, RenderContext::Config config)
    : logical_device_(device), window_(window), config_(config) {
  // create swapchain
  auto [width, height] = window_.getExtent();
  vk::Extent2D extent{width, height};
  swapchain_ = std::make_unique<Swapchain>(logical_device_,
                                           config_.surface,
                                           extent,
                                           config_.surface_format,
                                           config_.color_space,
                                           config_.present_mode);
  // prepare frame context
  prepareFrames();
}

void RenderContext::prepareFrames() {
  // pepare per-frame data
  for (auto& rawImage : swapchain_->getImages()) {
    frames_.emplace_back(std::make_unique<FrameContext>(logical_device_, rawImage));
  }
}
}  // namespace vk1
