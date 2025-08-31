#include "vk1/core/buffer.hpp"

namespace vk1 {
Buffer::Buffer(VmaAllocator allocator,
               StagePool& stage_pool,
               vk::BufferUsageFlags usage,
               uint32_t buffer_size)
    : vma_allocator_(allocator), stage_pool_(stage_pool), usage_(usage) {
  // make sure only 1 bit has been set in usage
  // assert(usage && (!usage & (usage - 0x1)));
  // create VkBuffer
  vk::BufferCreateInfo bufferCreateInfo{

      .size = buffer_size,
      .usage = usage | vk::BufferUsageFlagBits::eTransferDst,
  };
  VmaAllocationCreateInfo allocInfo{
      .usage = VMA_MEMORY_USAGE_GPU_ONLY,
  };
  auto result = vmaCreateBuffer(vma_allocator_,
                                reinterpret_cast<VkBufferCreateInfo*>(&bufferCreateInfo),
                                &allocInfo,
                                reinterpret_cast<VkBuffer*>(&vk_buffer_),
                                &vma_allocation_,
                                nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("buffer create failed!");
  }
}

Buffer::~Buffer() {
  vmaDestroyBuffer(vma_allocator_, static_cast<VkBuffer>(vk_buffer_), vma_allocation_);
}

void Buffer::uploadToGpu(vk::CommandBuffer cmd_buf,
                         const void* cpu_data,
                         uint32_t data_size,
                         uint32_t offset) {
  const Stage* stage = stage_pool_.acquireStage(data_size);
  void* mapped;
  vmaMapMemory(vma_allocator_, stage->memory, &mapped);
  std::memcpy(mapped, cpu_data, data_size);
  vmaUnmapMemory(vma_allocator_, stage->memory);
  // if memory is not coherent, must flush manually
  vmaFlushAllocation(vma_allocator_, stage->memory, offset, data_size);

  // TODO: barrier for sync

  // set buffer region
  vk::BufferCopy region{
      .srcOffset = 0,
      .dstOffset = offset,
      .size = data_size,
  };
  // copy data from cpu to gpu
  cmd_buf.copyBuffer(stage->buffer, vk_buffer_, 1, &region);
  // vk::vkCmdCopyBuffer(cmd_buf, stage->buffer, vk_buffer_, 1, &region);
}
}  // namespace vk1
