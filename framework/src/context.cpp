#define VOLK_IMPLEMENTATION
#include "vk1/core/context.hpp"

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  initVulkan();
}

Context::~Context() {}

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
  createSwapchain();
  createImageViews();
  createRenderPass();
  creategraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

void Context::createInstance() {}
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
void Context::createSwapchain() {}
void Context::createImageViews() {}
void Context::createRenderPass() {}
void Context::creategraphicsPipeline() {}
void Context::createFramebuffers() {}
void Context::createCommandPool() {}
void Context::createCommandBuffers() {}
void Context::createSyncObjects() {}

}  // namespace vk1
