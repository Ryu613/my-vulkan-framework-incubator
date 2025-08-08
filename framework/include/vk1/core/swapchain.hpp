#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct SwapchainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> surface_formats;
  std::vector<vk::PresentModeKHR> present_modes;
};
class Swapchain final {
 public:
  NO_COPY_MOVE(Swapchain);

  explicit Swapchain(const LogicalDevice& logical_device,
                     vk::SurfaceKHR surface,
                     vk::Extent2D extent,
                     vk::Format format = vk::Format::eR8G8B8A8Srgb,
                     vk::ColorSpaceKHR color_space = vk::ColorSpaceKHR::eSrgbNonlinear,
                     vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo);
  ~Swapchain();

 public:
  inline vk::Format getSurfaceFormat() const {
    return surface_format_;
  }

  inline uint32_t getImageCount() const {
    return static_cast<uint32_t>(images_.size());
  }

  inline const std::vector<vk::Image>& getImages() const {
    return images_;
  }

  inline vk::Extent2D getImageExtent() const {
    return extent_;
  }

  inline const std::vector<VkImageView>& getImageViews() const {
    return image_views_;
  }

  uint32_t acquireNextImage();

  VkSubmitInfo createSubmitInfo(VkCommandBuffer command_buffer,
                                const VkPipelineStageFlags* submit_wait_stage_flag) const;

  void recreate();

  void present(VkQueue present_queue);

 private:
  const LogicalDevice& logical_device_;
  vk::SwapchainKHR vk_swapchain_ = VK_NULL_HANDLE;
  vk::Extent2D extent_;
  vk::Format surface_format_;
  vk::ColorSpaceKHR color_space_;
  vk::PresentModeKHR present_mode_;
  std::vector<vk::Image> images_;
  std::vector<VkImageView> image_views_;
  uint32_t image_index_ = 0;
  VkSemaphore image_available_ = VK_NULL_HANDLE;
  VkSemaphore image_rendered_ = VK_NULL_HANDLE;
  VkFence acquire_fence_ = VK_NULL_HANDLE;

 private:
  SwapchainSupportDetails querySwapchainSupport(const PhysicalDevice& physical_device,
                                                vk::SurfaceKHR surface);
  SwapchainProps chooseSwapchainProperties(vk::Format required_surface_format,
                                           vk::ColorSpaceKHR required_color_space,
                                           vk::PresentModeKHR required_present_mode,
                                           const SwapchainSupportDetails& supported);
  vk::Extent2D chooseExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR& capabilities);
  void wrapSwapchainImages();
  void createSyncObjects();
};
}  // namespace vk1
