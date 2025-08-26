#pragma once

#include "vk1/core/common.hpp"
#include "vma/vk_mem_alloc.h"

namespace vk1 {
struct Stage {
  VmaAllocation memory{VK_NULL_HANDLE};
  VkBuffer buffer{VK_NULL_HANDLE};
  uint32_t capacity{0};
  mutable uint64_t last_frame_count{0};
};
// manage staging buffer in pool to optimizing performance
class StagePool {
 public:
  StagePool(VmaAllocator allocator);

  // given number of bytes to create a stage buffer
  const Stage* acquireStage(uint32_t size);

  void gc() noexcept;

  void terminate() noexcept;

 private:
  // use for gc to calculate minimum gc time
  static constexpr uint64_t TIME_BEFORE_EVICTION = MAX_COMMAND_BUFFERS;
  VmaAllocator vma_allocator_;
  std::multimap<uint32_t, const Stage*> free_stages_;
  std::unordered_set<const Stage*> used_stages_;
  uint64_t current_frame_count_{0};
};
}  // namespace vk1
