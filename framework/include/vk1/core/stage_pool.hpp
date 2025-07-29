#pragma once

#include "vk1/core/common.hpp"
#include "vma/vk_mem_alloc.h"

namespace vk1 {
struct Stage {
  VmaAllocation memory;
  VkBuffer buffer;
  uint32_t capacity;
};
// manage staging buffer in pool to reuse for optimizing performance
class StagePool {
 public:
  StagePool(VmaAllocator allocator);

  // given number of bytes to create a stage buffer
  const Stage* acquireStage(uint32_t size);

  void gc() noexcept;

  void terminate() noexcept;

 private:
  VmaAllocator vma_allocator_;
  std::multimap<uint32_t, const Stage*> free_stages_;
};
}  // namespace vk1
