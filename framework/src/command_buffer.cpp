#include "vk1/core/command_buffer.hpp"

#include "vk1/core/logical_device.hpp"
#include "vk1/core/command_pool.hpp"

namespace vk1 {
CommandBuffer::CommandBuffer(const CommandPool& command_pool, vk::CommandBufferLevel level)
    : command_pool_(command_pool) {
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.commandPool = command_pool_.getVkCommandPool();
  allocInfo.level = level;
  allocInfo.commandBufferCount = 1;

  vk_command_buffer_ = command_pool_.getLogicalDevice().getVkDevice().allocateCommandBuffers(allocInfo);
}

CommandBuffer::~CommandBuffer() {
  auto& device = command_pool_.getLogicalDevice().getVkDevice();
  device.freeCommandBuffers(command_pool_.getVkCommandPool(), vk_command_buffer_);
}
}  // namespace vk1
