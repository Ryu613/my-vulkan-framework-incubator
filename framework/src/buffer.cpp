#include "vk1/core/buffer.hpp"

namespace vk1 {
Buffer::Buffer(VmaAllocator allocator, StagePool& stage_pool, VkBufferUsageFlags usage, uint32_t buffer_size)
    : vma_allocator_(allocator), stage_pool_(stage_pool), usage_(usage) {
  // make sure only 1 bit has been set in usage
  assert(usage && (!usage & (usage - 1)));
  // create VkBuffer
  VkBufferCreateInfo bufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                      .size = buffer_size,
                                      .usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
  VmaAllocationCreateInfo allocInfo{
      .usage = VMA_MEMORY_USAGE_GPU_ONLY,
  };
  auto result =
      vmaCreateBuffer(vma_allocator_, &bufferCreateInfo, &allocInfo, &vk_buffer_, &vma_allocation_, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("buffer create failed!");
  }
}

Buffer::~Buffer() {
  vmaDestroyBuffer(vma_allocator_, vk_buffer_, vma_allocation_);
}

void Buffer::uploadToGpu(VkCommandBuffer cmd_buf, const void* cpu_data, uint32_t data_size, uint32_t offset) {
  const Stage* stage = stage_pool_.acquireStage(data_size);
  void* mapped;
  vmaMapMemory(vma_allocator_, stage->memory, &mapped);
  std::memcpy(mapped, cpu_data, data_size);
  vmaUnmapMemory(vma_allocator_, stage->memory);
  // if memory is not coherent, must flush manually
  vmaFlushAllocation(vma_allocator_, stage->memory, offset, data_size);

  // TODO: barrier for sync

  // set buffer region
  VkBufferCopy region{
      .srcOffset = 0,
      .dstOffset = offset,
      .size = data_size,
  };
  // copy data from cpu to gpu
  vkCmdCopyBuffer(cmd_buf, stage->buffer, vk_buffer_, 1, &region);
}
}  // namespace vk1
