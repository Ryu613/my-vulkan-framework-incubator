#pragma once

#include <memory>
#include <string>
#include <vector>

#include "vk1/common/common.hpp"
#include "vk1/core/instance.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"

namespace vk1 {
struct ContextConfig {
  std::string app_name;
  void* window = nullptr;
  bool is_debug = false;
  std::vector<const char*> required_layers;
  std::vector<const char*> required_instance_extensions;
  std::vector<const char*> required_device_extensions;
};

class Context final {
 public:
  MOVABLE_ONLY(Context);

  explicit Context(const ContextConfig& config);

  ~Context();

 private:
  ContextConfig config_;
  std::unique_ptr<Instance> instance_;
  std::unique_ptr<LogicalDevice> logical_device_;
  VkSurfaceKHR surface_;

  void initVulkan();
  void createInstance();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapchain();
  void createImageViews();
  void createRenderPass();
  void creategraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();

  bool checkDeviceExtensionSupport();
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
};
}  // namespace vk1
