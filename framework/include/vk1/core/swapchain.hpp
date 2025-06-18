#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> surface_formats;
  std::vector<VkPresentModeKHR> present_modes;
};
class Swapchain final {
 public:
  NO_COPY_MOVE(Swapchain);

  explicit Swapchain(const LogicalDevice& logical_device,
                     VkSurfaceKHR surface,
                     VkExtent2D extent,
                     VkFormat format = VK_FORMAT_UNDEFINED,
                     VkColorSpaceKHR color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                     VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR);
  ~Swapchain();

 public:
  inline VkFormat getSurfaceFormat() const {
    return surface_format_;
  }

  inline uint32_t getImageCount() const {
    return static_cast<uint32_t>(images_.size());
  }

 private:
  const LogicalDevice& logical_device_;
  VkSwapchainKHR vk_swapchain_ = VK_NULL_HANDLE;
  VkExtent2D extent_;
  VkFormat surface_format_;
  VkColorSpaceKHR color_space_;
  VkPresentModeKHR present_mode_;
  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  VkSemaphore image_available_ = VK_NULL_HANDLE;
  VkSemaphore image_rendered_ = VK_NULL_HANDLE;
  VkFence acquire_fence_ = VK_NULL_HANDLE;

 private:
  SwapchainSupportDetails querySwapchainSupport(const PhysicalDevice& physical_device, VkSurfaceKHR surface);
  SwapchainProps chooseSwapchainProperties(VkFormat required_surface_format,
                                           VkColorSpaceKHR required_color_space,
                                           VkPresentModeKHR required_present_mode,
                                           const SwapchainSupportDetails& supported);
  VkExtent2D chooseExtent(VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities);
  void createImageViews();
  void createSyncObjects();
};
}  // namespace vk1
