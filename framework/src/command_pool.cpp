#include "vk1/core/command_pool.hpp"

#include "vk1/core/logical_device.hpp"

namespace vk1 {
CommandPool::CommandPool(const LogicalDevice& device, uint32_t queue_family_index) : logical_device_(device) {
  vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queue_family_index,
  };
  vk_command_pool_ = logical_device_.getVkDevice().createCommandPool(poolInfo);
}

CommandPool::~CommandPool() {
  logical_device_.getVkDevice().destroyCommandPool(vk_command_pool_);
}
}  // namespace vk1
