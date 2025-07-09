#pragma once
// core
#include "vk1/core/common.hpp"
#include "vk1/core/frame_buffer.hpp"
#include "vk1/core/image.hpp"
#include "vk1/core/instance.hpp"
#include "vk1/core/logical_device.hpp"
#include "vk1/core/physical_device.hpp"
#include "vk1/core/render_pass.hpp"
#include "vk1/core/swapchain.hpp"
// platform
#include "vk1/platform/window.hpp"
// bottom
#include "vma/vk_mem_alloc.h"

namespace vk1 {
class RenderContext;
struct ContextConfig {
  std::string app_name;
  Window* window = nullptr;
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
  // void createRenderContext();
  void drawFrame();

  inline const VmaAllocator& getAllocator() const {
    return allocator_;
  }

  std::shared_ptr<Image> createImage(const ImageConfig& config) const;

 private:
  ContextConfig config_;
  std::unique_ptr<Instance> instance_;
  std::unique_ptr<LogicalDevice> logical_device_;
  // std::unique_ptr<RenderContext> render_context_;
  VkSurfaceKHR surface_;
  VmaAllocator allocator_ = nullptr;
  std::unique_ptr<RenderPass> render_pass_;
  std::unique_ptr<Swapchain> swapchain_;
  std::vector<std::unique_ptr<FrameBuffer>> frame_buffers_;

 private:
  void initVulkan();

  void createSurface();
  void createMemoryAllocator();
  void createSwapchain();
  void createRenderPass();
  void createGraphicsPipeline();
  void createColorResources();
  void createDepthResources();
  void createFramebuffers();
  void createCommandPool();
  void createTextureImage();
  void createTextureImageView();
  void createTextureSampler();
  void loadModel();
  void createVertexBuffer();
  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();
  void createDescriptorSets();
  void createCommandPoolAndBuffers();
};
}  // namespace vk1
