#include "vk1/core/render_pass.hpp"

namespace vk1 {
RenderPass::RenderPass(const LogicalDevice& logical_device, vk::Format format)
    : Handle(logical_device, nullptr) {
  std::array<vk::AttachmentDescription, 2> attachments{
      // color
      {
          .format = format,
          .samples = vk::SampleCountFlagBits::e1,
          .loadOp = vk::AttachmentLoadOp::eClear,
          .storeOp = vk::AttachmentStoreOp::eDontCare,
          .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
          .initialLayout = vk::ImageLayout::eUndefined,
          .finalLayout = vk::ImageLayout::ePresentSrcKHR,
      },
      // depth
      {
          .format = findDepthFormat(),
          .samples = vk::SampleCountFlagBits::e1,
          .loadOp = vk::AttachmentLoadOp::eClear,
          .storeOp = vk::AttachmentStoreOp::eDontCare,
          .stencilLoadOp = vk::AttachmentLoadOp::eClear,
          .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
          .initialLayout = vk::ImageLayout::eUndefined,
          .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
      },
  };

  vk::AttachmentReference colorRef{0, vk::ImageLayout::eColorAttachmentOptimal};
  vk::AttachmentReference depthRef{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

  vk::SubpassDescription subpassDescrption{
      .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorRef,
      .pDepthStencilAttachment = &depthRef,
  };

  std::array<vk::SubpassDependency, 2> dependencies{
      {
          .srcSubPass = VK_SUBPASS_EXTERNAL,
          .dstSubpass = 0,
          .srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe,
          .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                          vk::PipelineStageFlagBits::eEarlyFragmentTests |
                          vk::PipelineStageFlagBits::eLateFragmentTests,
          .srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                           vk::AccessFlagBits::eColorAttachmentWrite |
                           vk::AccessFlagBits::eDepthStencilAttachmentRead |
                           vk::AccessFlagBits::eDepthStencilAttachmentWrite,
          .dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                           vk::AccessFlagBits::eColorAttachmentWrite |
                           vk::AccessFlagBits::eDepthStencilAttachmentRead |
                           vk::AccessFlagBits::eDepthStencilAttachmentWrite,
          .dependencyFlags = vk::DependencyFlagBits::eByRegion,
      },
      {
          .srcSubpass = 0,
          .dstSubpass = VK_SUBPASS_EXTERNAL,
          .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                          vk::PipelineStageFlagBits::eEarlyFragmentTests |
                          vk::PipelineStageFlagBits::eLateFragmentTests,
          .dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe,
          .srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                           vk::AccessFlagBits::eColorAttachmentWrite |
                           vk::AccessFlagBits::eDepthStencilAttachmentRead |
                           vk::AccessFlagBits::eDepthStencilAttachmentWrite,
          .dstAccessMask = vk::AccessFlagBits::eMemoryRead,
          .dependencyFlags = vk::DependencyFlagBits::eByRegion,
      },
  };

  vk::RenderPassCreateInfo createInfo{
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &subpassDescrption,
      .dependencyCount = static_cast<uint32_t>(dependencies.size()),
      .pDependencies = dependencies.data(),
  };
  auto renderPass = getLogicalDevice().getVkDevice().createRenderPass(createInfo);
  setHandle(renderPass);
}

RenderPass::~RenderPass() {
  getLogicalDevice().getVkDevice().destroyRenderPass(getHandle());
}

VkFormat RenderPass::findDepthFormat() {
  return findSupportedFormat(
      {
          vk::Format::eD32Sfloat,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint,
      },
      vk::ImageTiling::eOptimal,
      vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

VkFormat RenderPass::findSupportedFormat(const std::vector<vk::Format>& candidates,
                                         vk::ImageTiling tiling,
                                         vk::FormatFeatureFlags features) {
  vk::PhysicalDevice phyDevice = logical_device_.getPhysicalDevice().getVkPhysicalDevice();
  for (vk::Format format : candidates) {
    vk::FormatProperties props = phyDevice.getFormatProperties(format);
    if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}
}  // namespace vk1
