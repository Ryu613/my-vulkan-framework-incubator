#pragma once

#include "vk1/common/common.hpp"

#ifdef _WIN32
#if !defined(VK_USE_PLATFORM_WIN32_KHR)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif

#define VK_NO_PROTOTYPES
#include "glm/glm.hpp"
#include "vulkan/vulkan.hpp"

namespace vk1 {
using OptionalExtensions = std::unordered_map<const char*, bool>;
using OptionalLayers = OptionalExtensions;

struct SwapchainProps {
  vk::Format format;
  vk::ColorSpaceKHR color_space;
  vk::PresentModeKHR present_mode;
};

inline const OptionalLayers DEFAULT_DEBUG_LAYERS = {{"VK_LAYER_KHRONOS_validation", true}};
inline const OptionalExtensions DEFAULT_DEBUG_INSTANCE_EXTENSIONS = {
    {VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true},
};
inline const OptionalExtensions DEFAULT_INSTANCE_EXTENSIONS = {
    {VK_KHR_SURFACE_EXTENSION_NAME, true},
};
inline const OptionalExtensions DEFAULT_DEVICE_EXTENSIONS = {
    {VK_KHR_SWAPCHAIN_EXTENSION_NAME, true},
};
inline const std::map<uint32_t, SwapchainProps> DEFAULT_SWAPCHAIN_PROPS_PRIORITY_LIST = {
    {
        1U,
        {
            vk::Format::eR8G8B8A8Srgb,
            vk::ColorSpaceKHR::eSrgbNonlinear,
            vk::PresentModeKHR::eFifo,
        },
    },
};

// 3: triple bufferring
// 15: maximum number of renderpasses
// in-flight: command submitted but gpu may(or not yet) executed
// credit: filament
constexpr static int MAX_IN_FLIGHT_COMMAND_BUFFERS = 3 * 15;
}  // namespace vk1
