#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
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

 private:
  const LogicalDevice& logical_device_;
  vk::CommandPool vk_command_pool_;
};
}  // namespace vk1
