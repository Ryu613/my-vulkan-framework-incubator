#include "vk1/core/swapchain.hpp"

namespace vk1 {
Swapchain::Swapchain(const LogicalDevice& logical_device,
                     VkSurfaceKHR surface,
                     VkExtent2D extent,
                     VkFormat format,
                     VkColorSpaceKHR color_space,
                     VkPresentModeKHR present_mode)
    : logical_device_(logical_device) {
  SwapchainSupportDetails swapchainSupport =
      querySwapchainSupport(logical_device_.getPhysicalDevice(), surface);
  SwapchainProps choosedProps =
      chooseSwapchainProperties(format, color_space, present_mode, swapchainSupport);
  surface_format_ = choosedProps.format;
  present_mode_ = choosedProps.present_mode;
  extent_ = chooseExtent(extent, swapchainSupport.capabilities);

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
  if (swapchainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapchainSupport.capabilities.maxImageCount) {
    imageCount = swapchainSupport.capabilities.maxImageCount;
  }
  VkSwapchainCreateInfoKHR createInfo{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = imageCount,
      .imageFormat = choosedProps.format,
      .imageColorSpace = choosedProps.color_space,
      .imageExtent = extent_,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
  };

  QueueFamilyInfo queueFamilies = logical_device_.getQueueFamilyInfo();
  uint32_t QueueFamilyIndices[] = {queueFamilies.graphics_queue_family_index.value(),
                                   queueFamilies.present_queue_family_index.value()};
  if (queueFamilies.graphics_queue_family_index.value() != queueFamilies.present_queue_family_index.value()) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = QueueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }
  createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = choosedProps.present_mode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  if (vkCreateSwapchainKHR(logical_device_.getVkDevice(), &createInfo, nullptr, &vk_swapchain_) !=
      VK_SUCCESS) {
    throw std::runtime_error("faield to create swapchain!");
  }

  vkGetSwapchainImagesKHR(logical_device_.getVkDevice(), vk_swapchain_, &imageCount, nullptr);
  images_.resize(imageCount);
  vkGetSwapchainImagesKHR(logical_device_.getVkDevice(), vk_swapchain_, &imageCount, images_.data());

  createImageViews();

  createSyncObjects();
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(logical_device_.getVkDevice(), vk_swapchain_, nullptr);
}

SwapchainSupportDetails Swapchain::querySwapchainSupport(const PhysicalDevice& physical_device,
                                                         VkSurfaceKHR surface) {
  VkPhysicalDevice device = physical_device.getVkPhysicalDevice();
  SwapchainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    details.surface_formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.surface_formats.data());
  }
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    details.present_modes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.present_modes.data());
  }
  return details;
}

SwapchainProps Swapchain::chooseSwapchainProperties(VkFormat required_surface_format,
                                                    VkColorSpaceKHR required_color_space,
                                                    VkPresentModeKHR required_present_mode,
                                                    const SwapchainSupportDetails& supported) {
  if (required_surface_format == VK_FORMAT_UNDEFINED) {
    for (const auto& [priority, props] : DEFAULT_SWAPCHAIN_PROPS_PRIORITY_LIST) {
      for (const auto& format : supported.surface_formats) {
        if (format.format == props.format && format.colorSpace == props.color_space) {
          for (const auto& presentMode : supported.present_modes) {
            if (presentMode == props.present_mode) {
              return props;
            }
          }
        }
      }
    }
    throw std::runtime_error("failed to choose suitable swapchain properties!");
  }
  for (const auto& each : supported.surface_formats) {
    if (each.format == required_surface_format && each.colorSpace == required_color_space) {
      for (const auto& presentMode : supported.present_modes) {
        if (presentMode == required_present_mode) {
          return {required_surface_format, required_color_space, required_present_mode};
        }
      }
    }
  }
  throw std::runtime_error("failed to choose suitable swapchain properties!");
}
VkExtent2D Swapchain::chooseExtent(VkExtent2D extent, const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {static_cast<uint32_t>(extent.width), static_cast<uint32_t>(extent.height)};
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actualExtent;
  }
}

void Swapchain::createImageViews() {
  image_views_.resize(images_.size());
  for (size_t i = 0; i < images_.size(); ++i) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = images_[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = surface_format_;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(logical_device_.getVkDevice(), &createInfo, nullptr, &image_views_[i]) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }
  }
}

void Swapchain::createSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
  };

  VkFenceCreateInfo fenceInfo{
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };

  if (vkCreateSemaphore(logical_device_.getVkDevice(), &semaphoreInfo, nullptr, &image_available_) !=
          VK_SUCCESS ||
      vkCreateSemaphore(logical_device_.getVkDevice(), &semaphoreInfo, nullptr, &image_rendered_) !=
          VK_SUCCESS ||
      vkCreateFence(logical_device_.getVkDevice(), &fenceInfo, nullptr, &acquire_fence_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create semaphores!");
  }
}

void Swapchain::acquireNextImage() {
  const auto& device = logical_device_.getVkDevice();
  vkWaitForFences(device, 1, &acquire_fence_, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &acquire_fence_);
  VkResult result = vkAcquireNextImageKHR(logical_device_.getVkDevice(),
                                          vk_swapchain_,
                                          UINT64_MAX,
                                          image_available_,
                                          VK_NULL_HANDLE,
                                          &image_index_);
  // TODO: framebuffer resize callback
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreate();
    return;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to acquire swapchain image!");
  }
}

void Swapchain::recreate() {}
}  // namespace vk1
