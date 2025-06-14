#pragma once
#include "vk1/common/common.hpp"
#include "vk1/core/instance.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"
#include "vma/vk_mem_alloc.h"

namespace vk1 {
class RenderContext;
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

 public:
  void createRenderContext();

 private:
  ContextConfig config_;
  std::unique_ptr<Instance> instance_;
  std::unique_ptr<LogicalDevice> logical_device_;
  std::unique_ptr<RenderContext> render_context_;
  VkSurfaceKHR surface_;
  VmaAllocator allocator_ = nullptr;

 private:
  void initVulkan();

  void createSurface();

  void createMemoryAllocator();
  void createSwapchain();
  void createImageViews();
  void createRenderPass();
  void creategraphicsPipeline();
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
};
}  // namespace vk1
