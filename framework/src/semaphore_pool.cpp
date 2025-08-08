#include "vk1/core/semaphore_pool.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
SemaphorePool::SemaphorePool(const LogicalDevice& device) : logical_device_(device) {}

SemaphorePool::~SemaphorePool() {}
}  // namespace vk1
