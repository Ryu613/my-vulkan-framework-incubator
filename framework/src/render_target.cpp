#include "vk1/rendering/render_target.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/image.hpp"
#include "vk1/core/image_view.hpp"

namespace vk1 {
RenderTarget::RenderTarget(std::vector<Image>&& render_images)
    : logical_device_(render_images.back().getLogicalDevice()), images_(std::move(render_images)) {
  // create image views & attachment for render images
  for (auto& image : images_) {
    image_views_.emplace_back(image, vk::ImageViewType::e2D);
    // TODO: attachment
  }
}

RenderTarget::~RenderTarget() {}
}  // namespace vk1
