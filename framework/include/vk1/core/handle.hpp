#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"

namespace vk1 {
// wrapping vulkan object handle and device accessor, ensure resource realease correctly
template <typename T>
class Handle {
 public:
  // no copy
  Handle(const Handle&) = delete;
  Handle& operator=(const Handle&) = delete;
  // can move
  Handle(Handle&& other) noexcept;
  Handle& operator=(Handle&& other) noexcept;
  // constructor
  Handle(LogicalDevice* device, T t);
  virtual ~Handle() = default;

 public:
  inline const T& getHandle() const {
    return handle_;
  }
  inline const LogicalDevice& getLogicalDevice() const {
    return *device_;
  }

  inline void setHandle(T t) {
    handle_ = t;
  }

 private:
  LogicalDevice* device_{nullptr};
  T handle_{nullptr};
};

template <typename T>
inline Handle<T>::Handle(LogicalDevice* device, T t) : device_(device), handle_(t) {}

template <typename T>
inline Handle<T>::Handle(Handle&& other) noexcept
    : device_(other.device_), handle_(std::exchange(other.handle_, {})) {}

template <typename T>
inline Handle<T>& Handle<T>::operator=(Handle&& other) noexcept {
  device_ = std::exchange(other.device_, {});
  handle_ = std::exchange(other.handle_, {});
  return *this;
}

}  // namespace vk1
