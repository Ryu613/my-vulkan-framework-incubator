#define VOLK_IMPLEMENTATION
#include "vk1/core/context.hpp"

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  initVulkan();
}

void Context::initVulkan() {
  createInstance();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapchain();
  createImageViews();
  createRenderPass();
  creategraphicsPipeline();
  createFramebuffers();
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

void Context::createInstance() {
  if (volkInitialize()) {
    throw std::runtime_error("failed to initialize volk!");
  }
  instance_ = std::make_unique<Instance>(
      config_.app_name, config_.required_layers, config_.required_instance_extensions, config_.is_debug);

  volkLoadInstance(instance_->getVkInstance());
  instance_->createPhysicalDevice();
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
void Context::pickPhysicalDevice() {
  PhysicalDevice
}
void Context::createLogicalDevice() {}
void Context::createSwapchain() {}
void Context::createImageViews() {}
void Context::createRenderPass() {}
void Context::creategraphicsPipeline() {}
void Context::createFramebuffers() {}
void Context::createCommandPool() {}
void Context::createCommandBuffers() {}
void Context::createSyncObjects() {}

bool Context::checkDeviceExtensionSupport() {
  // uint32_t extensionCount;
  // vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  // std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  // vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  // std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
  // for (const auto& extension : availableExtensions) {
  //   requiredExtensions.erase(extension.extensionName);
  // }
  // return requiredExtensions.empty();
}

}  // namespace vk1
