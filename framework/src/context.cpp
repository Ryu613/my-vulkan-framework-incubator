#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "vk1/core/context.hpp"

#include "vk1/core/buffer.hpp"
#include "vk1/support/gltf_loader.hpp"
#include "vk1/support/model.hpp"
#include "ze/utils/filesystem.hpp"

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  ze::utils::filesystem::init();
  initVulkan();
}

Context::~Context() {
  auto device = logical_device_->getVkDevice();
  vkDeviceWaitIdle(device);
  for (auto& frameBuffer : frame_buffers_) {
    frameBuffer.reset();
  }
  swapchain_.reset();
  render_pass_.reset();
  logical_device_.reset();
  vmaDestroyAllocator(allocator_);
  if (surface_ != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instance_->getVkInstance(), surface_, nullptr);
  }
  instance_.reset();
}

void Context::initVulkan() {
  // create instance
  instance_ = std::make_unique<Instance>(
      config_.app_name, config_.required_layers, config_.required_instance_extensions, config_.is_debug);
  // create surface
  surface_ = config_.window->createSurface(*instance_);
  // choose physical device
  const auto& suitablePhysicalDevice =
      instance_->chooseSuitablePhysicalDevice(surface_, config_.required_device_extensions);
  // create logical device
  logical_device_ = std::make_unique<LogicalDevice>(suitablePhysicalDevice, surface_);
  createMemoryAllocator();
  createSwapchain();
  createRenderPass();
  createCommandPoolAndBuffers();
  createGraphicsPipeline();
  // create color & depth resource
  const auto& swapchainFormat = swapchain_->getSurfaceFormat();
  const auto& imageExtent = swapchain_->getImageExtent();
  auto colorImage = createImage({
      .width = imageExtent.width,
      .height = imageExtent.height,
      .mipLevels = 1,
      .format = swapchainFormat,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  });

  auto depthImage = createImage({
      .width = imageExtent.width,
      .height = imageExtent.height,
      .mipLevels = 1,
      .format = VK_FORMAT_D24_UNORM_S8_UINT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  });
  // create frame buffers
  uint32_t imageCount = swapchain_->getImageCount();
  const auto& imageViews = swapchain_->getImageViews();
  frame_buffers_.reserve(imageCount);
  for (uint32_t i = 0; i < imageCount; ++i) {
    std::vector<VkImageView> attachments;
    attachments.push_back(colorImage->getVkImageView());
    attachments.push_back(depthImage->getVkImageView());
    attachments.push_back(imageViews[i]);
    auto frameBuffer = std::make_unique<FrameBuffer>(
        *logical_device_, render_pass_->getVkRenderPass(), imageExtent, attachments);
    frame_buffers_.push_back(std::move(frameBuffer));
  }
  // create sampler
  auto sampler = logical_device_->createSampler();
  // load model
  GLTFLoader gltfLoader;
  auto model = gltfLoader.loadModel("models/aaaa.gltf");
  // create vertex buffer
  auto vertexBuffer =
      std::make_unique<Buffer>(allocator_,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               model->vertices_count_ * sizeof(Vertex));
  model->vertex_buffer_ = std::move(vertexBuffer);
  // model->vertex_buffer_->uploadToGpu();
}

void Context::createMemoryAllocator() {
  const uint32_t apiVersion = VK_API_VERSION_1_3;
  const VmaVulkanFunctions vulkanFunctions = {
      .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
#if VMA_VULKAN_VERSION >= 1003000
      .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
      .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
#endif
  };

  const VmaAllocatorCreateInfo allocInfo = {
      .physicalDevice = logical_device_->getPhysicalDevice().getVkPhysicalDevice(),
      .device = logical_device_->getVkDevice(),
      .pVulkanFunctions = &vulkanFunctions,
      .instance = instance_->getVkInstance(),
      .vulkanApiVersion = apiVersion,
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
void Context::createGraphicsPipeline() {
  Pipeline::PipelineConfig pipeConfig{
      .pipelineType = Pipeline::PipelineType::GraphicsPipeline,
      .vertexShaderPath = "Shaders/vert.spv",
      .fragmentShaderPath = "Shaders/frag.spv",
      .renderPass = render_pass_->getVkRenderPass(),
      .viewportExtent = swapchain_->getImageExtent(),
  };
  logical_device_->createPipeline(pipeConfig);
}

void Context::createCommandPoolAndBuffers() {
  uint32_t graphicsQueueFamilyIndex =
      logical_device_->getQueueFamilyInfo().graphics_queue_family_index.value();
  logical_device_->createCommandPoolAndBuffers(graphicsQueueFamilyIndex, swapchain_->getImageCount());
}
void Context::drawFrame() {
  uint32_t imageIndex = swapchain_->acquireNextImage();
  VkCommandBuffer cmdBuffer = logical_device_->getCommandBufferToBegin();
  // begin render pass
  auto extent = swapchain_->getImageExtent();
  const auto& pipeline = logical_device_->getPipeline();
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass_->getVkRenderPass();
  renderPassInfo.framebuffer = frame_buffers_[imageIndex]->getVkFrameBuffer();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = extent;
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipeline());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;
  vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

  vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(cmdBuffer);
  logical_device_->endCommandBuffer(cmdBuffer);
  constexpr VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  const auto submitInfo = swapchain_->createSubmitInfo(cmdBuffer, &flags);
  logical_device_->submitCommand(cmdBuffer, submitInfo);
  swapchain_->present(logical_device_->getPresentQueue());
}

std::shared_ptr<Image> Context::createImage(const ImageConfig& config) const {
  return std::make_shared<Image>(*this, config);
}
}  // namespace vk1
