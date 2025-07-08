#include "vk1/core/buffer.hpp"

#include "vk1/core/context.hpp"

namespace vk1 {
Buffer::Buffer(const Context& context, VkDeviceSize size, VkBufferUsageFlags usage)
    : context_(context), size_(size), usage_(usage) {
  VkBufferCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = {},
      .size = size_,
      .usage = usage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = {},
      .pQueueFamilyIndices = {},
  };
  vma_allocation_create_info_ = {
      VMA_ALLOCATION_CREATE_MAPPED_BIT,
      VMA_MEMORY_USAGE_CPU_ONLY,
  };
  const auto& allocator = context_.getAllocator();
  auto result = vmaCreateBuffer(
      allocator, &createInfo, &vma_allocation_create_info_, &vk_buffer_, &vma_allocation_, nullptr);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  vmaGetAllocationInfo(allocator, vma_allocation_, &vma_allocation_info);
}
}  // namespace vk1
