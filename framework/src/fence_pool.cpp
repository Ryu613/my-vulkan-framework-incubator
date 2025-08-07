#include "vk1/core/fence_pool.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
FencePool::FencePool(const LogicalDevice& device) : logical_device_(device) {}

FencePool::~FencePool() {}

vk::Fence FencePool::acquireFence() {}

vk::Result FencePool::reset() {}
}  // namespace vk1
