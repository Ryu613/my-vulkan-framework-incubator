#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VULKAN_HPP_
#include "vk1/core/context.hpp"

#include "vk1/core/allocation.hpp"
#include "vk1/core/buffer.hpp"
#include "vk1/rendering/render_context.hpp"
#include "vk1/rendering/frame_context.hpp"
#include "vk1/rendering/render_target.hpp"
#include "vk1/support/gltf_loader.hpp"
#include "vk1/support/model.hpp"
#include "ze/utils/filesystem.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vk1 {

Context::Context(const ContextConfig& config) : config_(config) {
  ze::utils::filesystem::init();
  initVulkan();
}

Context::~Context() {
  auto device = logical_device_->getVkDevice();
  device.waitIdle();
  for (auto& frameBuffer : frame_buffers_) {
    frameBuffer.reset();
  }
  // swapchain_.reset();
  render_pass_.reset();
  logical_device_.reset();
  // vmaDestroyAllocator(allocator_);
  if (surface_ != VK_NULL_HANDLE) {
    instance_->getVkInstance().destroySurfaceKHR(surface_);
  }
  instance_.reset();
}

void Context::initVulkan() {
  static vk::detail::DynamicLoader dl;
  VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
  // create instance
  instance_ = std::make_unique<Instance>(
      config_.app_name, config_.required_layers, config_.required_instance_extensions, config_.is_debug);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(instance_->getVkInstance());
  // create surface
  surface_ = config_.window->createSurface(*instance_);
  if (!surface_) {
    throw std::runtime_error("failed to  create window surface");
  }
  // choose physical device
  const auto& suitablePhysicalDevice =
      instance_->chooseSuitablePhysicalDevice(surface_, config_.required_device_extensions);
  // create logical device
  logical_device_ = std::make_unique<LogicalDevice>(suitablePhysicalDevice, surface_);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(logical_device_->getVkDevice());
  // vma allocator
  createMemoryAllocator();
  // create render context
  render_context_ = std::make_unique<RenderContext>(*logical_device_, surface_, config_.window);

  // Swapchain buffer?
  // synchronization primitives?

  // create render pass
  auto format = render_context_->getSwapchain().getSurfaceFormat();
  render_pass_ = std::make_unique<RenderPass>(*logical_device_, format);
  // create frame buffers
  frame_buffers_.clear();
  frame_buffers_.resize(render_context_->getRenderFrames().size());
  auto imageExtent = render_context_->getSwapchain().getImageExtent();
  std::array<vk::ImageView, 2> attachments;
  for (uint32_t i = 0; i < frame_buffers_.size(); ++i) {
    attachments[0] = render_context_->getRenderFrames()[i]->getRenderTarget().getImageViews()[0];
    attachments[1] = render_context_->getRenderFrames()[i]->getRenderTarget().getImageViews()[1];

    auto frameBuffer =
        std::make_unique<FrameBuffer>(*logical_device_, render_pass_->getHandle(), imageExtent, attachments);
    frame_buffers_.push_back(std::move(frameBuffer));
  }
  // create graphics pipeline
  Pipeline::Config pipeConfig{
      .pipelineType = Pipeline::Type::GraphicsPipeline,
      .vertexShaderPath = "Shaders/vert.spv",
      .fragmentShaderPath = "Shaders/frag.spv",
      .renderPass = render_pass_->getHandle(),
      .viewportExtent = swapchain_->getImageExtent(),
  };
  logical_device_->createPipeline(pipeConfig);
  // create sampler
  auto sampler = logical_device_->createSampler();
  // load model
  GLTFLoader gltfLoader;
  auto model = gltfLoader.loadModel("models/aaaa.gltf");
  // create vertex buffer
  auto vertexBuffer =
      std::make_unique<Buffer>(allocator::get(),
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               model->vertices_count_ * sizeof(Vertex));
  model->vertex_buffer_ = std::move(vertexBuffer);
  // model->vertex_buffer_->uploadToGpu();
}

void Context::createMemoryAllocator() {
  const uint32_t apiVersion = VK_API_VERSION_1_3;
  const VmaVulkanFunctions vulkanFunctions = {
      .vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr,
      .vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr,
  };

  const VmaAllocatorCreateInfo allocInfo = {
      .pVulkanFunctions = &vulkanFunctions,
      .physicalDevice =
          static_cast<VkPhysicalDevice>(logical_device_->getPhysicalDevice().getVkPhysicalDevice()),
      .device = static_cast<VkDevice>(logical_device_->getVkDevice()),
      .instance = static_cast<VkInstance>(instance_->getVkInstance()),
  };
  allocator::init(allocInfo);
}  // namespace vk1

// void Context::createSwapchain() {
//   auto [width, height] = config_.window->getExtent();
//   VkExtent2D extent{width, height};
//   swapchain_ = std::make_unique<Swapchain>(*logical_device_, surface_, extent);
// }
// void Context::createRenderPass() {
//   auto format = render_context_->getSwapchain().getSurfaceFormat();
//   render_pass_ = std::make_unique<RenderPass>(*logical_device_, format);
// }

// void Context::createCommandPoolAndBuffers() {
//   uint32_t graphicsQueueFamilyIndex =
//       logical_device_->getQueueFamilyInfo().graphics_queue_family_index.value();
//   logical_device_->createCommandPoolAndBuffers(graphicsQueueFamilyIndex, swapchain_->getImageCount());
// }

void Context::drawFrame() {
  // uint32_t imageIndex = swapchain_->acquireNextImage();
  // VkCommandBuffer cmdBuffer = logical_device_->getCommandBufferToBegin();
  // // begin render pass
  // auto extent = swapchain_->getImageExtent();
  // const auto& pipeline = logical_device_->getPipeline();
  // VkRenderPassBeginInfo renderPassInfo{};
  // renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  // renderPassInfo.renderPass = render_pass_->getVkRenderPass();
  // renderPassInfo.framebuffer = frame_buffers_[imageIndex]->getVkFrameBuffer();
  // renderPassInfo.renderArea.offset = {0, 0};
  // renderPassInfo.renderArea.extent = extent;
  // VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  // renderPassInfo.clearValueCount = 1;
  // renderPassInfo.pClearValues = &clearColor;

  // vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  // vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVkPipeline());

  // VkViewport viewport{};
  // viewport.x = 0.0f;
  // viewport.y = 0.0f;
  // viewport.width = static_cast<float>(extent.width);
  // viewport.height = static_cast<float>(extent.height);
  // viewport.minDepth = 0.0f;
  // viewport.maxDepth = 1.0f;
  // vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

  // VkRect2D scissor{};
  // scissor.offset = {0, 0};
  // scissor.extent = extent;
  // vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

  // vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

  // vkCmdEndRenderPass(cmdBuffer);
  // logical_device_->endCommandBuffer(cmdBuffer);
  // constexpr VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // const auto submitInfo = swapchain_->createSubmitInfo(cmdBuffer, &flags);
  // logical_device_->submitCommand(cmdBuffer, submitInfo);
  // swapchain_->present(logical_device_->getPresentQueue());
}

}  // namespace vk1
