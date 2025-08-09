#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
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
  Handle(const LogicalDevice& device, T t);
  virtual ~Handle() = default;

 public:
  inline const T& getHandle() const {
    return handle_;
  }
  inline const LogicalDevice& getLogicalDevice() const {
    return device_;
  }

  inline void setHandle(T t) {
    handle_ = t;
  }

 private:
  const LogicalDevice& device_;
  T handle_{nullptr};
};

template <typename T>
inline Handle<T>::Handle(const LogicalDevice& device, T t) : device_(device), handle_(T) {}

template <typename T>
inline Handle<T>::Handle(Handle&& other) noexcept
    : device_(other.device_), handle(std::exchange(other.handle, {})) {}

template <typename T>
inline Handle<T>& Handle<T>::operator=(Handle&& other) noexcept {
  device_ = other.device_;
  handle = std::exchange(other.handle, {});
  return *this;
}

}  // namespace vk1
