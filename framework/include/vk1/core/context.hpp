#pragma once

#include "vk1/common/common.hpp"
#include "vk1/core/instance.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct ContextConfig {
  std::string app_name;
  void* window = nullptr;
  bool is_debug = false;
  OptionalLayers required_layers;
  OptionalExtensions required_instance_extensions;
  OptionalExtensions required_device_extensions;
  uint32_t vulkan_api_version = VK_API_VERSION_1_0;
};

class Context final {
 public:
  MOVABLE_ONLY(Context);

  explicit Context(const ContextConfig& config);

  ~Context();

 private:
  ContextConfig config_;
  std::unique_ptr<Instance> instance_;
  VkSurfaceKHR surface_;
  std::optional<uint32_t> graphics_queue_family_index_;
  std::optional<uint32_t> present_queue_family_index_;
  std::unique_ptr<LogicalDevice> logical_device_;

 private:
  void initVulkan();
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice(const PhysicalDevice& physical_device);
  void createSwapchain();
  void createImageViews();
  void createRenderPass();
  void creategraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
  void populateQueueFamilyIndex(const PhysicalDevice& physical_device);
};
}  // namespace vk1
