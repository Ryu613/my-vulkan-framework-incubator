#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/handle.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
class Image;
class ImageView {
 public:
  struct Config {};

 public:
  NO_COPY_MOVE(ImageView);

  ImageView(const Image& image, vk::ImageViewType view_type, vk::Format format = vk::Format::eUndefined);

  ~ImageView();

 public:
 private:
  const Image& image_;
  vk::ImageView vk_image_view_;
  vk::Format format_;
}
}  // namespace vk1
