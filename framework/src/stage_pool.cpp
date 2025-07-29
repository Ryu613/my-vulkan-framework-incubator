#include "vk1/core/stage_pool.hpp"

namespace vk1 {
StagePool::StagePool(VmaAllocator allocator) : vma_allocator_(allocator) {}

const Stage* StagePool::acquireStage(uint32_t size) {
  auto iter = free_stages_.lower_bound(size);
}
}  // namespace vk1
