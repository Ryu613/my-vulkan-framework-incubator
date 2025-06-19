#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"

namespace vk1 {
class FrameBuffer final {
 public:
  NO_COPY_MOVE(FrameBuffer);

  explicit FrameBuffer(const LogicalDevice& logical_device,
                       VkRenderPass render_pass,
                       VkExtent2D extent,
                       const std::vector<VkImageView>& attachments);

  ~FrameBuffer();

 public:
  inline VkFramebuffer getVkFrameBuffer() const {
    return vk_frame_buffer_;
  }

 private:
  const LogicalDevice& logical_device_;
  VkFramebuffer vk_frame_buffer_ = VK_NULL_HANDLE;
};
}  // namespace vk1
