#include "vk1/rendering/render_target.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/image.hpp"

namespace vk1 {
RenderTarget::RenderTarget(const LogicalDevice& device, std::vector<Image>&& render_images)
    : logical_device_(device), images_(render_images) {
  // create image views for render images
  // create attachment
}

RenderTarget::~RenderTarget() {}
}  // namespace vk1
