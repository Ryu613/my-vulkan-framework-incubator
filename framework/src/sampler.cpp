#include "vk1/core/sampler.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
Sampler::Sampler(const LogicalDevice& device) : logical_device_(device) {
  vk::SamplerCreateInfo samplerInfo{
      .magFilter = vk::Filter::eLinear,
      .minFilter = vk::Filter::eLinear,
      .addressModeU = vk::SamplerAddressMode::eRepeat,
      .addressModeV = vk::SamplerAddressMode::eRepeat,
      .addressModeW = vk::SamplerAddressMode::eRepeat,
      .anisotropyEnable = VK_TRUE,
  };
  vk::PhysicalDeviceProperties properties = logical_device_.getPhysicalDevice().getProperties();

  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = vk::CompareOp::eAlways;

  samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;

  vk_sampler_ = logical_device_.getVkDevice().createSampler(samplerInfo);
}

Sampler::~Sampler() {
  logical_device_.getVkDevice().destroySampler(vk_sampler_);
}
}  // namespace vk1
