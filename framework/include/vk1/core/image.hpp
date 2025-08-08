#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class Image {
 public:
  struct Config {
    // to be used when image is created by swapchain or elsewhere
    vk::Image image;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 0;
    VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
    vk::Format format;
    vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
    vk::ImageUsageFlags usageFlags;
    vk::MemoryPropertyFlags memoryPropertyFlags;
  };

 public:
  MOVABLE_ONLY(Image);

  Image(const LogicalDevice& device, vk::Image vk_image);

 private:
  const LogicalDevice& device;
};
}  // namespace vk1
