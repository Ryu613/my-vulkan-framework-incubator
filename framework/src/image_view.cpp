#include "vk1/core/image_view.hpp"

#include <vulkan/vulkan_format_traits.hpp>

#include "vk1/core/image.hpp"

namespace vk1 {
ImageView::ImageView(const Image& image, vk::ImageViewType view_type, vk::Format format)
    : image_(image), format_(format) {
  // create vk image view
  if (format == vk::Format::eUndefined) {
    format_ = image.getConfig().format;
  }
  vk::ImageSubresourceRange subresource{};
  subresource.aspectMask = std::string(vk::componentName(format, 0)) == "D" ? vk::ImageAspectFlagBits::eDepth
                                                                            : vk::ImageAspectFlagBits::eColor;
  subresource.baseMipLevel = 0;
  subresource.levelCount = image.getConfig().mipLevels;
  subresource.baseArrayLayer = 0;
  subresource.layerCount = 1;

  vk::ImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.image = image_.getHandle();
  imageViewCreateInfo.viewType = view_type;
  imageViewCreateInfo.format = format_;
  imageViewCreateInfo.subresourceRange = subresource;

  vk_image_view_ = image.getLogicalDevice().getVkDevice().createImageView(imageViewCreateInfo);
}

ImageView::~ImageView() {
  image_.getLogicalDevice().getVkDevice().destroyImageView(vk_image_view_);
}
}  // namespace vk1
