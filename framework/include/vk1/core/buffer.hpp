#pragma once

#include "vk1/core/common.hpp"
#include "vma/vk_mem_alloc.h"

namespace vk1 {
class Context;
class Buffer final {
 public:
  NO_COPY_MOVE(Buffer);

  explicit Buffer(const Context& context,
                  VkDeviceSize size,
                  VkBufferUsageFlags buffer_usage,
                  VmaMemoryUsage mem_usage);

  ~Buffer();

 public:
  inline VkDeviceSize getSize() const {
    return size_;
  }

  void uploadToGPU(VkDeviceSize size, VkDeviceSize offset) const;

  void copyDataToBuffer(const void* data, size_t size) const;

 private:
  const Context& context_;
  VkDeviceSize size_{};
  VkBufferUsageFlags usage_{};
  VmaAllocationCreateInfo vma_allocation_create_info_{};
  VmaAllocation vma_allocation_ = nullptr;
  VmaAllocationInfo vma_allocation_info{};
  VkBuffer vk_buffer_{VK_NULL_HANDLE};
  mutable void* mapped_memory_ = nullptr;
};
}  // namespace vk1
