#include "vk1/core/sampler.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
Sampler::Sampler(const LogicalDevice& device) : logical_device_(device) {
  VkSamplerCreateInfo samplerInfo{
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .anisotropyEnable = VK_TRUE,
  };
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(logical_device_.getPhysicalDevice().getVkPhysicalDevice(), &properties);

  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;

  if (vkCreateSampler(logical_device_.getVkDevice(), &samplerInfo, nullptr, &vk_sampler_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

Sampler::~Sampler() {
  vkDestroySampler(logical_device_.getVkDevice(), vk_sampler_, nullptr);
}
}  // namespace vk1
