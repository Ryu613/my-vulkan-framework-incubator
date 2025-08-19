#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/handle.hpp"
#include "vk1/core/logical_device.hpp"

namespace vk1 {
class RenderPass final : public Handle<vk::RenderPass> {
 public:
  NO_COPY_MOVE(RenderPass);

  RenderPass(const LogicalDevice& logical_device, vk::Format format);

  ~RenderPass();

 private:
  VkFormat findDepthFormat();
  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
};
}  // namespace vk1
