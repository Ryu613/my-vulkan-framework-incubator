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
  vk::ImageCreateInfo createInfo{.format = config_.format,
                                 .mipLevels = config_.mipLevels,
                                 .samples = config_.numSamples,
                                 .extent = config_.extent,
                                 .imageType = vk::ImageType::e2D,
                                 .arrayLayers = 1,
                                 .tiling = vk::ImageTiling::eOptimal,
                                 .usage = config_.usageFlags,
                                 .sharingMode = vk::SharingMode::eExclusive};
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
