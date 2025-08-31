#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/handle.hpp"
#include "vma/vk_mem_alloc.h"

namespace vk1 {
namespace allocator {
// vma allocator
VmaAllocator& get();
void init(const VmaAllocatorCreateInfo& create_info);
void destroy();
}  // namespace allocator

template <typename T>
class Allocation : public Handle<T> {
 public:
  using ParentType = Handle<T>;

 public:
  MOVABLE_ONLY(Allocation);

 public:
  const T* get() const;

  /**
   * @brief flush memory if it is not HOST_COHERENT, otherwise do nothing
   */
  void flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

  /**
   * @brief get pointer of host visible memory
   */
  const uint8_t* getDataPtr() const;

  /**
   * @brief get raw vulkan memory object
   */
  vk::DeviceMemory getMemory() const;

  /**
   * @brief map memory to a host visible address, if already mapped or persistent, do nothing
   */
  uint8_t* map();

  /**
   * @brief true if memory is mapped
   */
  inline bool mapped() const {
    return mapped_data != nullptr;
  }

  /**
   * @brief unmap memory from the host visible address, do nothing if not mapped or persistent
   */
  void unmap();

 protected:
  template <typename... Args>
  inline Allocation(const VmaAllocationCreateInfo& alloc_create_info, Args&&... args)
      : Handle<T>{std::forward<Args>(args)...} {
    alloc_create_info_.flags = alloc_create_info.flags;
    alloc_create_info_.memoryTypeBits = alloc_create_info.memoryTypeBits;
    alloc_create_info_.pool = alloc_create_info.pool;
    alloc_create_info_.preferredFlags = alloc_create_info.preferredFlags;
    alloc_create_info_.priority = alloc_create_info.priority;
    alloc_create_info_.pUserData = alloc_create_info.pUserData;
    alloc_create_info_.requiredFlags = alloc_create_info.requiredFlags;
    alloc_create_info_.usage = alloc_create_info.usage;
  }

 protected:
  /**
   * @brief create image using vma allocator
   */
  [[nodiscard]] vk::Image createImage(const vk::ImageCreateInfo& create_info);

  /**
   * @brief destroy image
   */
  void destroyImage(vk::Image image);

  /**
   * @brief clear internal state
   */
  void clear();

 private:
  VmaAllocationCreateInfo alloc_create_info_{};
  VmaAllocation alloc_{VK_NULL_HANDLE};
  uint8_t* mapped_data{nullptr};
  bool coherent{false};
  bool persistent{false};
};

template <typename T>
vk::Image Allocation<T>::createImage(const vk::ImageCreateInfo& create_info) {
  vk::Image image = VK_NULL_HANDLE;
  VmaAllocationInfo alloc_info{};
  auto result = vmaCreateImage(allocator::get(),
                               reinterpret_cast<const VkImageCreateInfo*>(&create_info),
                               &alloc_create_info_,
                               reinterpret_cast<VkImage*>(&image),
                               &alloc_,
                               &alloc_info);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("cannot allocate image");
  }
  VkMemoryPropertyFlags memProps;
  vmaGetAllocationMemoryProperties(allocator::get(), alloc_, &memProps);
  coherent = (memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  mapped_data = static_cast<uint8_t*>(alloc_info.pMappedData);
  persistent = mapped();
}

template <typename T>
inline void Allocation<T>::destroyImage(vk::Image image) {
  if (image && alloc_ != VK_NULL_HANDLE) {
    unmap();
    vmaDestroyImage(allocator::get(), static_cast<VkImage>(image), alloc_);
    clear();
  }
}

template <typename T>
void Allocation<T>::clear() {
  mapped_data = nullptr;
  persistent = false;
  alloc_create_info_ = {};
}
}  // namespace vk1
