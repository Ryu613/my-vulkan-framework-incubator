#include "vk1/core/image.hpp"

#include "vk1/core/logical_device.hpp"
#include "vma/vk_mem_alloc.h"
namespace vk1 {
Image::Image(const LogicalDevice& device, Image::Config config)
    : Allocation(buildAllocCreateInfo(config), device, nullptr), config_(config) {
  if (config_.image) {
    setHandle(config_.image);
    return;
  }
  vk::ImageCreateInfo createInfo{
      .imageType = vk::ImageType::e2D,
      .format = config_.format,
      .extent = config_.extent,
      .mipLevels = config_.mipLevels,
      .arrayLayers = 1,
      .samples = config_.numSamples,
      .tiling = vk::ImageTiling::eOptimal,
      .usage = config_.usageFlags,
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
  createInfo.usage = config.memoryUsage;
  return createInfo;
}
}  // namespace vk1
