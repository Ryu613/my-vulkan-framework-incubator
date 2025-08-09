#include "vk1/core/allocation.hpp"

namespace vk1 {
namespace allocator {
VmaAllocator& get() {
  static VmaAllocator allocator = VK_NULL_HANDLE;
  return allocator;
}
void init(const VmaAllocatorCreateInfo& create_info) {
  auto& allocator = get();
  if (allocator == VK_NULL_HANDLE) {
    vmaCreateAllocator(&cerate_info, &allocator);
  }
}
void destroy() {
  auto& allocator = get();
  if (allocator != VK_NULL_HANDLE) {
    vmaDestroyAllocator(allocator);
    allocator = VK_NULL_HANDLE;
  }
}
}  // namespace allocator
}  // namespace vk1
