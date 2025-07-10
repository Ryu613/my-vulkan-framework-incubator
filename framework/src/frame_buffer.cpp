#include "vk1/core/frame_buffer.hpp"

namespace vk1 {
FrameBuffer::FrameBuffer(const LogicalDevice& logical_device,
                         VkRenderPass render_pass,
                         VkExtent2D extent,
                         const std::vector<VkImageView>& attachments)
    : logical_device_(logical_device) {
  VkFramebufferCreateInfo framebufferInfo{
      .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
      .renderPass = render_pass,
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .width = extent.width,
      .height = extent.height,
      .layers = 1,
  };

  if (vkCreateFramebuffer(logical_device_.getVkDevice(), &framebufferInfo, nullptr, &vk_frame_buffer_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer!");
  }
}

FrameBuffer::~FrameBuffer() {
  vkDestroyFramebuffer(logical_device_.getVkDevice(), vk_frame_buffer_, nullptr);
}
}  // namespace vk1
