#include "vk1/rendering/render_target.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/image.hpp"

namespace vk1 {
RenderTarget::RenderTarget(const LogicalDevice& device) : logical_device_(device) {}

RenderTarget::~RenderTarget() {}
}  // namespace vk1
