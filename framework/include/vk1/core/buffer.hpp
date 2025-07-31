#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/stage_pool.hpp"

namespace vk1 {
class Buffer {
 public:
  explicit Buffer(VmaAllocator allocator,
                  StagePool& stage_pool,
                  VkBufferUsageFlags usage,
                  uint32_t buffer_size);
  ~Buffer();

 public:
  void uploadToGpu(VkCommandBuffer cmd_buf, const void* cpu_data, uint32_t data_size, uint32_t offset);

  inline VkBuffer getVkBuffer() const {
    return vk_buffer_;
  }

 private:
  VmaAllocator vma_allocator_{VK_NULL_HANDLE};
  StagePool& stage_pool_;
  VmaAllocation vma_allocation_{VK_NULL_HANDLE};
  VkBuffer vk_buffer_{VK_NULL_HANDLE};
  VkBufferUsageFlags usage_{};
};
}  // namespace vk1
