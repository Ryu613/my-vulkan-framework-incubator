#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Image;
class ImageView;
class RenderTarget {
 public:
  NO_COPY_MOVE(RenderTarget);

  RenderTarget(std::vector<Image>&& render_images);

  ~RenderTarget();

 private:
  const LogicalDevice& logical_device_;
  std::vector<Image> images_;
  std::vector<ImageView> image_views_;
};
}  // namespace vk1
