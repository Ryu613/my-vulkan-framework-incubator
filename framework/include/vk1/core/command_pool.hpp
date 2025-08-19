#pragma once

#include "vk1/core/common.hpp"

#include <bitset>

namespace vk1 {
class LogicalDevice;
class CommandBuffer;
class CommandPool {
 public:
  NO_COPY_MOVE(CommandPool);

  CommandPool(const LogicalDevice& device, uint32_t queue_family_index);

  ~CommandPool();

 public:
  inline const LogicalDevice& getLogicalDevice() const {
    return logical_device_;
  }

  inline vk::CommandPool getVkCommandPool() const {
    return vk_command_pool_;
  }

  CommandBuffer& acquireCommandBuffer(vk::CommandBufferLevel level, uint32_t count);

 private:
  static constexpr int CAPACITY = MAX_COMMAND_BUFFERS;
  const LogicalDevice& logical_device_;
  vk::CommandPool vk_command_pool_;
  std::vector<std::unique_ptr<CommandBuffer>> primary_command_buffers_;
  std::bitset<CAPACITY> primary_active_status_;
};
}  // namespace vk1
