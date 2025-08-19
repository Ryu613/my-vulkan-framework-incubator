#include "vk1/core/command_pool.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/command_buffer.hpp"

namespace vk1 {
CommandPool::CommandPool(const LogicalDevice& device, uint32_t queue_family_index) : logical_device_(device) {
  // create command pool
  vk::CommandPoolCreateInfo poolInfo{
      .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      .queueFamilyIndex = queue_family_index,
  };
  vk_command_pool_ = logical_device_.getVkDevice().createCommandPool(poolInfo);
  // create command buffers
  for (size_t i = 0; i < CAPACITY; ++i) {
    primary_command_buffers_.emplace_back(
        std::make_unique<CommandBuffer>(*this, vk::CommandBufferLevel::ePrimary));
  }
}

CommandPool::~CommandPool() {
  logical_device_.getVkDevice().destroyCommandPool(vk_command_pool_);
}

CommandBuffer& CommandPool::acquireCommandBuffer(vk::CommandBufferLevel level, uint32_t count) {}
}  // namespace vk1
