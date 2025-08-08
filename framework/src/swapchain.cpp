#include "vk1/core/swapchain.hpp"

#include "vk1/common/util.hpp"

namespace vk1 {
Swapchain::Swapchain(const LogicalDevice& logical_device,
                     vk::SurfaceKHR surface,
                     vk::Extent2D extent,
                     vk::Format format,
                     vk::ColorSpaceKHR color_space,
                     vk::PresentModeKHR present_mode)
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
  vk::SwapchainCreateInfoKHR createInfo{
      .surface = surface,
      .minImageCount = imageCount,
      .imageFormat = choosedProps.format,
      .imageColorSpace = choosedProps.color_space,
      .imageExtent = extent_,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
  };

  QueueFamilyInfo queueFamilies = logical_device_.getQueueFamilyInfo();
  uint32_t QueueFamilyIndices[] = {queueFamilies.graphics_queue_family_index.value(),
                                   queueFamilies.present_queue_family_index.value()};
  if (queueFamilies.graphics_queue_family_index.value() != queueFamilies.present_queue_family_index.value()) {
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = QueueFamilyIndices;
  } else {
    createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }
  createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  createInfo.presentMode = choosedProps.present_mode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  // create swapchain
  vk_swapchain_ = logical_device_.getVkDevice().createSwapchainKHR(createInfo);
  // get swapchain images
  images_ = logical_device_.getVkDevice().getSwapchainImagesKHR(vk_swapchain_);

  // wrapSwapchainImages();

  // createSyncObjects();
}

Swapchain::~Swapchain() {
  if (vk_swapchain_) {
    logical_device_.getVkDevice().destroySwapchainKHR(vk_swapchain_);
  }
  // const auto& device = logical_device_.getVkDevice();
  // vkWaitForFences(device, 1, &acquire_fence_, VK_TRUE, UINT64_MAX);
  // vkDestroyFence(device, acquire_fence_, nullptr);
  // vkDestroySemaphore(device, image_rendered_, nullptr);
  // vkDestroySemaphore(device, image_available_, nullptr);
  // for (auto imageView : image_views_) {
  //   vkDestroyImageView(logical_device_.getVkDevice(), imageView, nullptr);
  // }
  // vkDestroySwapchainKHR(logical_device_.getVkDevice(), vk_swapchain_, nullptr);
}

SwapchainSupportDetails Swapchain::querySwapchainSupport(const PhysicalDevice& physical_device,
                                                         vk::SurfaceKHR surface) {
  const auto& device = physical_device.getVkPhysicalDevice();
  SwapchainSupportDetails details{
      .capabilities = device.getSurfaceCapabilitiesKHR(surface),
      .surface_formats = device.getSurfaceFormatsKHR(surface),
      .present_modes = device.getSurfacePresentModesKHR(surface),
  };
  return details;
}

SwapchainProps Swapchain::chooseSwapchainProperties(vk::Format required_surface_format,
                                                    vk::ColorSpaceKHR required_color_space,
                                                    vk::PresentModeKHR required_present_mode,
                                                    const SwapchainSupportDetails& supported) {
  if (required_surface_format == vk::Format::eUndefined) {
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
vk::Extent2D Swapchain::chooseExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    vk::Extent2D actualExtent = {util::castUInt32(extent.width), util::castUInt32(extent.height)};
    actualExtent.width =
        std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actualExtent;
  }
}

void Swapchain::wrapSwapchainImages() {
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

uint32_t Swapchain::acquireNextImage() {
  const auto& device = logical_device_.getVkDevice();
  vkWaitForFences(device, 1, &acquire_fence_, VK_TRUE, UINT64_MAX);
  VkResult result = vkAcquireNextImageKHR(logical_device_.getVkDevice(),
                                          vk_swapchain_,
                                          UINT64_MAX,
                                          image_available_,
                                          VK_NULL_HANDLE,
                                          &image_index_);
  // TODO: framebuffer resize callback
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreate();
    return image_index_;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to acquire swapchain image!");
  }
  vkResetFences(device, 1, &acquire_fence_);
  return image_index_;
}

void Swapchain::recreate() {
  throw std::runtime_error("not implemented yet!");
}

VkSubmitInfo Swapchain::createSubmitInfo(VkCommandBuffer command_buffer,
                                         const VkPipelineStageFlags* submit_wait_stage_flag) const {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  // VkSemaphore waitSemaphores[] = {image_available_};
  // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &image_available_;
  submitInfo.pWaitDstStageMask = submit_wait_stage_flag;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffer;

  // VkSemaphore signalSemaphores[] = {image_rendered_};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &image_rendered_;
  return submitInfo;
}

void Swapchain::present(VkQueue present_queue) {
  VkSemaphore signalSemaphores[] = {image_rendered_};
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapchains[] = {vk_swapchain_};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &image_index_;
  presentInfo.pResults = nullptr;
  vkQueuePresentKHR(present_queue, &presentInfo);
}
}  // namespace vk1
