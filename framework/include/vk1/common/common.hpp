#pragma once

#ifdef _WIN32
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif

#include <algorithm>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
