#include "vk1/core/fence_pool.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
FencePool::FencePool(const LogicalDevice& device) : logical_device_(device) {}

FencePool::~FencePool() {
  wait();
  reset();

  for (vk::Fence fence : fences_) {
    logical_device_.getVkDevice().destroyFence(fence);
  }

  fences_.clear();
}

vk::Fence FencePool::acquireFence() {
  if (active_fence_count < fences_.size()) {
    return fences_[active_fence_count++];
  }

  vk::Fence fence = logical_device_.getVkDevice().createFence({});
  fences_.push_back(fence);
  active_fence_count++;
  return fences_.back();
}

vk::Result FencePool::reset() {
  if (active_fence_count < 1 || fences_.empty()) {
    return vk::Result::eSuccess;
  }
  logical_device_.getVkDevice().resetFences(active_fence_count, fences_.data());
  active_fence_count = 0;
  return vk::Result::eSuccess;
}

vk::Result FencePool::wait(uint64_t timeout) const {
  if (active_fence_count < 1 || fences_.empty()) {
    return vk::Result::eSuccess;
  }
  return logical_device_.getVkDevice().waitForFences(active_fence_count, fences_.data(), true, timeout);
}
}  // namespace vk1
