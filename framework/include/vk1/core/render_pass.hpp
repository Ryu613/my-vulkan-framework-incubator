#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"

namespace vk1 {
class RenderPass final {
 public:
  NO_COPY_MOVE(RenderPass);

  explicit RenderPass(const LogicalDevice& logical_device, VkFormat format);

  ~RenderPass();

 public:
  inline const VkRenderPass& getVkRenderPass() const {
    return vk_render_pass_;
  }

 private:
  const LogicalDevice& logical_device_;
  VkRenderPass vk_render_pass_;
};
}  // namespace vk1
