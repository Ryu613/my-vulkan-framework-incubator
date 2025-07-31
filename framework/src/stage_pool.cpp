#include "vk1/core/stage_pool.hpp"

namespace vk1 {
StagePool::StagePool(VmaAllocator allocator) : vma_allocator_(allocator) {}

const Stage* StagePool::acquireStage(uint32_t size) {
  auto iter = free_stages_.lower_bound(size);
  // re-use free stages if possible
  if (iter != free_stages_.end()) {
    auto stage = iter->second;
    free_stages_.erase(iter);
    stage->last_frame_count = current_frame_count_;
    used_stages_.insert(stage);
    return stage;
  }
  // if no free stage available, then create one
  Stage* stage = new Stage({
      .memory = VK_NULL_HANDLE,
      .buffer = VK_NULL_HANDLE,
      .capacity = size,
      .last_frame_count = current_frame_count_,
  });
  used_stages_.insert(stage);
  // create VkBuffer using VMA
  VkBufferCreateInfo bufferInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = size,
      .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
  };
  VmaAllocationCreateInfo allocInfo{
      .usage = VMA_MEMORY_USAGE_CPU_ONLY,
  };
  auto result =
      vmaCreateBuffer(vma_allocator_, &bufferInfo, &allocInfo, &stage->buffer, &stage->memory, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate stage buffer memory!");
  }
  return stage;
}

void StagePool::gc() noexcept {
  // return if too early
  if (++current_frame_count_ <= TIME_BEFORE_EVICTION) {
    return;
  }

  const uint64_t evictionTime = current_frame_count_ - TIME_BEFORE_EVICTION;
  // destroy buffers which have not been used for several frames
  decltype(free_stages_) freeStages;
  // use swap to avoid iterator & operation error
  freeStages.swap(free_stages_);
  for (auto [capacity, stage] : freeStages) {
    if (stage->last_frame_count < evictionTime) {
      vmaDestroyBuffer(vma_allocator_, stage->buffer, stage->memory);
    } else {
      free_stages_.insert({capacity, stage});
    }
  }
  // update used stages's last frame count, if not accessed for several frames, then move this stage to free
  // stages list
  decltype(used_stages_) usedStages;
  usedStages.swap(used_stages_);
  for (auto each : usedStages) {
    if (each->last_frame_count < evictionTime) {
      each->last_frame_count = current_frame_count_;
      free_stages_.insert({each->capacity, each});
    } else {
      used_stages_.insert(each);
    }
  }
}

void StagePool::terminate() noexcept {
  for (auto stage : used_stages_) {
    vmaDestroyBuffer(vma_allocator_, stage->buffer, stage->memory);
    delete stage;
  }
  used_stages_.clear();

  for (auto [capacity, stage] : free_stages_) {
    vmaDestroyBuffer(vma_allocator_, stage->buffer, stage->memory);
    delete stage;
  }
  free_stages_.clear();
}
}  // namespace vk1
