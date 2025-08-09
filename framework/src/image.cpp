#include "vk1/core/image.hpp"

#include "vk1/core/logical_device.hpp"
#include "vma/vk_mem_alloc.h"
struct Config {
  // to be used when image is created by swapchain or elsewhere
  vk::Image image;
  vk::Extent3D extent = {0, 0, 1};
  uint32_t mipLevels = 0;
  VkSampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1;
  vk::Format format;
  vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
  vk::ImageUsageFlags usageFlags;
  vk::MemoryPropertyFlags memoryPropertyFlags;
};
namespace vk1 {
Image::Image(const LogicalDevice& device, Image::Config config)
    : Allocation(buildAllocCreateInfo(config), device, nullptr), config_(config) {
  if (config_.image) {
    setHandle(config_.image);
    return;
  }
  vk::ImageCreateInfo createInfo{
      .format = config_.format,
      .mipLevels = config_.mipLevels,
      .samples = config_.numSamples,
      .extent = config_.extent,
      .imageType = vk::ImageType::e2D,
      .arrayLayers = 1,
      .tiling = vk::ImageTiling::eOptimal,
      .usage = usageFlags,
      .sharingMode = vk::SharingMode::eExclusive,
  };
  setHandle(createImage(createInfo));
}

Image::~Image() {
  destroyImage(getHandle());
}

// helper function
VmaAllocationCreateInfo buildAllocCreateInfo(const Image::Config& config) {
  VmaAllocationCreateInfo createInfo{};
  createInfo.usage = VMA_MEMORY_USAGE_AUTO;
  return createInfo;
}
}  // namespace vk1
