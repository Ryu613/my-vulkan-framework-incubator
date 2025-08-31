#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"

namespace vk1 {
class RenderPass;
class FrameBuffer final {
 public:
  NO_COPY_MOVE(FrameBuffer);

  FrameBuffer(const LogicalDevice& logical_device,
              const RenderPass& render_pass,
              vk::Extent2D extent,
              const std::vector<vk::ImageView>& attachments);

  ~FrameBuffer();

 public:
  inline VkFramebuffer getVkFrameBuffer() const {
    return vk_frame_buffer_;
  }

 private:
  const LogicalDevice& logical_device_;
  vk::FrameBuffer vk_frame_buffer_;
};
}  // namespace vk1
