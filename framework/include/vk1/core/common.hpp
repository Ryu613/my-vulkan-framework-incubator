#pragma once

#include "vk1/common/common.hpp"

#define VK_NO_PROTOTYPES
#include <Volk/volk.h>

#ifdef _WIN32
#include <vulkan/vk_enum_string_helper.h>
#endif

namespace vk1 {
using OptionalExtensions = std::unordered_map<const char*, bool>;
using OptionalLayers = OptionalExtensions;

struct SwapchainProps {
  VkFormat format;
  VkColorSpaceKHR color_space;
  VkPresentModeKHR present_mode;
};

const OptionalLayers DEFAULT_DEBUG_LAYERS = {{"VK_LAYER_KHRONOS_validation", true}};
const OptionalExtensions DEFAULT_DEBUG_INSTANCE_EXTENSIONS = {{VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true}};
const OptionalExtensions DEFAULT_INSTANCE_EXTENSIONS = {{VK_KHR_SURFACE_EXTENSION_NAME, true}};
const OptionalExtensions DEFAULT_DEVICE_EXTENSIONS = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, true}};
const std::map<uint32_t, SwapchainProps> DEFAULT_SWAPCHAIN_PROPS_PRIORITY_LIST = {
    {1U, {VK_FORMAT_R8G8B8A8_SRGB, VK_COLORSPACE_SRGB_NONLINEAR_KHR, VK_PRESENT_MODE_FIFO_KHR}},
};
}  // namespace vk1
