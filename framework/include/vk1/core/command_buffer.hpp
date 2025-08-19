#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class CommandPool;
class CommandBuffer {
 public:
  NO_COPY_MOVE(CommandBuffer);

  CommandBuffer(const CommandPool& command_pool, vk::CommandBufferLevel level);

  ~CommandBuffer();

 private:
  const CommandPool& command_pool_;
  vk::CommandBuffer vk_command_buffer_;
};
}  // namespace vk1
