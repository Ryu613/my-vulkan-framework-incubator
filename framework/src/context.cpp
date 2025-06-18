#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "vk1/core/context.hpp"

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  initVulkan();
}

Context::~Context() {
  vkDeviceWaitIdle(logical_device_->getVkDevice());
  vmaDestroyAllocator(allocator_);
  if (surface_ != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance_->getVkInstance(), surface_, nullptr);
  }
}

void Context::initVulkan() {
  // create instance
  instance_ = std::make_unique<Instance>(
      config_.app_name, config_.required_layers, config_.required_instance_extensions, config_.is_debug);
  // create surface
  createSurface();
  // choose physical device
  const auto& suitablePhysicalDevice =
      instance_->chooseSuitablePhysicalDevice(surface_, config_.required_device_extensions);
  // create logical device
  logical_device_ = std::make_unique<LogicalDevice>(suitablePhysicalDevice, surface_);
  createMemoryAllocator();
  createSwapchain();
  createRenderPass();
  createCommandPoolAndBuffers();
  creategraphicsPipeline();
  createFramebuffers();
}
void Context::createSurface() {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  VkWin32SurfaceCreateInfoKHR createInfo{.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                         .hinstance = GetModuleHandle(NULL),
                                         .hwnd = (HWND)config_.window};
  if (vkCreateWin32SurfaceKHR(instance_->getVkInstance(), &createInfo, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create surface!");
  }
#else
  throw std::runtime_error("cannot create surface!");
#endif
}

void Context::createMemoryAllocator() {
  const VmaVulkanFunctions vulkanFunctions = {
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
  };

  const VmaAllocatorCreateInfo allocInfo = {
      .physicalDevice = logical_device_->getPhysicalDevice().getVkPhysicalDevice(),
      .device = logical_device_->getVkDevice(),
      .pVulkanFunctions = &vulkanFunctions,
      .instance = instance_->getVkInstance(),
  };
  vmaCreateAllocator(&allocInfo, &allocator_);
}

// void Context::createRenderContext() {
//   render_context_ = std::make_unique<RenderContext>(*logical_device_);
// }

void Context::createSwapchain() {
  auto [width, height] = config_.window->getExtent();
  VkExtent2D extent{width, height};
  swapchain_ = std::make_unique<Swapchain>(*logical_device_, surface_, extent);
}
void Context::createRenderPass() {
  VkFormat format = swapchain_->getSurfaceFormat();
  render_pass_ = std::make_unique<RenderPass>(*logical_device_, format);
}
void Context::creategraphicsPipeline() {}
void Context::createFramebuffers() {}
void Context::createCommandPoolAndBuffers() {
  uint32_t graphicsQueueFamilyIndex =
      logical_device_->getQueueFamilyInfo().graphics_queue_family_index.value();
  logical_device_->createCommandPoolAndBuffers(graphicsQueueFamilyIndex, swapchain_->getImageCount());
}

}  // namespace vk1
