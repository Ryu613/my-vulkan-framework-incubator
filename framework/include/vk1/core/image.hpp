#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class Context;
struct ImageConfig {
  // to be used when image is created by swapchain or elsewhere
  VkImage image{VK_NULL_HANDLE};
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t mipLevels = 0;
  VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
  VkFormat format{};
  VkImageTiling tiling{VK_IMAGE_TILING_OPTIMAL};
  VkImageUsageFlags usage{};
  VkMemoryPropertyFlags properties{};
};

class Image final {
 public:
  MOVABLE_ONLY(Image);
  explicit Image(const Context& context, const ImageConfig& image_config);
  ~Image();

 public:
 private:
  const Context& context_;
  ImageConfig imageConfig{};
  VkImage vk_image_{VK_NULL_HANDLE};
  VkImageView vk_image_view_{VK_NULL_HANDLE};
}
}  // namespace vk1
