#pragma once
#ifdef _WIN32
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define VK_NO_PROTOTYPES
#include <Volk/volk.h>

#ifdef _WIN32
#include <vulkan/vk_enum_string_helper.h>
#endif

#define MOVABLE_ONLY(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = default;       \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = default;

#define NO_COPY_MOVE(CLASS_NAME)                     \
  CLASS_NAME(const CLASS_NAME&) = delete;            \
  CLASS_NAME& operator=(const CLASS_NAME&) = delete; \
  CLASS_NAME(CLASS_NAME&&) noexcept = delete;        \
  CLASS_NAME& operator=(CLASS_NAME&&) noexcept = delete;

namespace vk1 {
using OptionalExtensions = std::unordered_map<const char*, bool>;
using OptionalLayers = OptionalExtensions;

const OptionalLayers DEFAULT_DEBUG_LAYERS = {{"VK_LAYER_KHRONOS_validation", true}};
const OptionalExtensions DEFAULT_DEBUG_INSTANCE_EXTENSIONS = {{VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true}};
const OptionalExtensions DEFAULT_INSTANCE_EXTENSIONS = {{VK_KHR_SURFACE_EXTENSION_NAME, true}};
const OptionalExtensions DEFAULT_DEVICE_EXTENSIONS = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, true}};
}  // namespace vk1
