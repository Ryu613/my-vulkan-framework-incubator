#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/allocation.hpp"

namespace vk1 {
class LogicalDevice;
class Image final : public Allocation<vk::Image> {
 public:
  struct Config {
    // to be used when image is created by swapchain or elsewhere
    vk::Image image;
    vk::Extent3D extent = {0, 0, 1};
    uint32_t mipLevels = 0;
    VkSampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
    vk::ImageType imageType = vk::ImageType::e2D;
    vk::Format format;
    vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
    vk::ImageUsageFlags usageFlags;
    vk::MemoryPropertyFlags memoryPropertyFlags;
  };

 public:
  MOVABLE_ONLY(Image);

  Image(const LogicalDevice& device, Image::Config config);

  ~Image();

 private:
  Image::Config config_;
};

// helper function
VmaAllocationCreateInfo buildAllocCreateInfo(const Image::Config& config);
}  // namespace vk1
