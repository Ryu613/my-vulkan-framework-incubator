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
  std::vector<Image> images;
  auto [width, height] = swapchain_->getImageExtent();
  // create color & depth image
  vk::Format depthFormat = logical_device_.getPhysicalDevice().findSupportedFormat(
      {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment);
  Image::Config depthImageConfig{
      .extent = {width, height, 1},
      .format = depthFormat,
      .usageFlags =
          vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
      .memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY,
  };
  Image::Config colorImageConfig{
      .extent = {width, height, 1},
      .format = swapchain_->getSurfaceFormat(),
      .usageFlags = vk::ImageUsageFlagBits::eColorAttachment,
  };
  for (auto& rawImage : swapchain_->getImages()) {
    colorImageConfig.image = rawImage;
    Image colorImage(logical_device_, colorImageConfig);
    Image depthImage(logical_device_, depthImageConfig);
    images.push_back(std::move(colorImage));
    images.push_back(std::move(colorImage));
    frames_.emplace_back(std::make_unique<FrameContext>(logical_device_, std::move(images)));
  }
}
}  // namespace vk1
