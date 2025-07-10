#include "vk1/core/image.hpp"

#include "vk1/core/context.hpp"

namespace vk1 {
Image::Image(const Context& context, const ImageConfig& image_config)
    : context_(context), image_config_(image_config), owns_vk_image_(true) {
  VkImageCreateInfo imageInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .flags = 0,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = image_config_.format,
      .extent = {image_config_.width, image_config_.height, 1},
      .mipLevels = image_config_.mipLevels,
      .arrayLayers = 1,
      .samples = image_config_.numSamples,
      .tiling = image_config_.tiling,
      .usage = image_config_.usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
  };

  Image::~Image() {
    const auto& device = context_.getLogicalDevice().getVkDevice();
    vkDestroyImageView(device, vk_image_view_, nullptr);
    if (owns_vk_image_) {
      vmaDestroyImage(context_.getAllocator(), vk_image_, vma_allocation_);
    }
  }

  const VmaAllocationCreateInfo allocCreateInfo{
      .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
      .usage = image_config_.properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                   ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST
                   : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
      .priority = 1.0f,
  };
  auto result = vmaCreateImage(
      context_.getAllocator(), &imageInfo, &allocCreateInfo, &vk_image_, &vma_allocation_, nullptr);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  if (vma_allocation_ != nullptr) {
    VmaAllocationInfo allocationInfo;
    vmaGetAllocationInfo(context_.getAllocator(), vma_allocation_, &allocationInfo);
    device_size_ = allocationInfo.size;
  }

  VkImageAspectFlags aspectMask{};
  if (isDepth()) {
    aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  } else if (isStencil()) {
    aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
  } else {
    aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  vk_image_view_ = createImageView(vk_image_, image_config_.format, aspectMask, image_config_.mipLevels);
}

VkImageView Image::createImageView(VkImage image,
                                   VkFormat format,
                                   VkImageAspectFlags aspectFlags,
                                   uint32_t mipLevels) {
  VkImageViewCreateInfo viewInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .subresourceRange =
          {
              .aspectMask = aspectFlags,
              .baseMipLevel = 0,
              .levelCount = mipLevels,
              .baseArrayLayer = 0,
              .layerCount = 1,
          },
  };

  const auto& device = context_.getLogicalDevice().getVkDevice();

  VkImageView imageView;
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return imageView;
}
}  // namespace vk1
