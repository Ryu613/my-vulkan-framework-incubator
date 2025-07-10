#pragma once

#include "vk1/core/common.hpp"
#include "vma/vk_mem_alloc.h"

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
  inline bool isDepth() const {
    return (
        image_config_.format == VK_FORMAT_D16_UNORM || image_config_.format == VK_FORMAT_D16_UNORM_S8_UINT ||
        image_config_.format == VK_FORMAT_D24_UNORM_S8_UINT || image_config_.format == VK_FORMAT_D32_SFLOAT ||
        image_config_.format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        image_config_.format == VK_FORMAT_X8_D24_UNORM_PACK32);
  }

  inline bool isStencil() const {
    return (image_config_.format == VK_FORMAT_S8_UINT ||
            image_config_.format == VK_FORMAT_D16_UNORM_S8_UINT ||
            image_config_.format == VK_FORMAT_D24_UNORM_S8_UINT ||
            image_config_.format == VK_FORMAT_D32_SFLOAT_S8_UINT);
  }

  inline VkImage getVkImage() const {
    return vk_image_;
  }

  inline VkImageView getVkImageView() const {
    return vk_image_view_;
  }

 private:
  const Context& context_;
  ImageConfig image_config_{};
  VmaAllocation vma_allocation_{nullptr};
  VkDeviceSize device_size_{0};
  bool owns_vk_image_{false};
  VkImage vk_image_{VK_NULL_HANDLE};
  VkImageView vk_image_view_{VK_NULL_HANDLE};

 private:
  VkImageView createImageView(VkImage image,
                              VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);
};
}  // namespace vk1
