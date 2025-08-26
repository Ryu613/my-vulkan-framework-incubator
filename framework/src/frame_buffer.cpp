#include "vk1/core/frame_buffer.hpp"

namespace vk1 {
FrameBuffer::FrameBuffer(const LogicalDevice& logical_device,
                         const RenderPass& render_pass,
                         vk::Extent2D extent,
                         const std::vector<vk::ImageView>& attachments)
    : logical_device_(logical_device) {
  vk::FramebufferCreateInfo frameBufferCreateInfo{
      .renderPass = render_pass_->getVkRenderPass(),
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .width = imageExtent.width,
      .height = imageExtent.height,
      .layers = 1,
  };
  vk_frame_buffer_ = logical_device_.getVkDevice().createFramebuffer(frameBufferCreateInfo);
}

FrameBuffer::~FrameBuffer() {
  logical_device_.getVkDevice().destroyFramebuffer(vk_frame_buffer_);
}
}  // namespace vk1
