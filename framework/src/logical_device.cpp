#include "vk1/core/logical_device.hpp"

#include "vk1/core/sampler.hpp"
#include "vk1/core/swapchain.hpp"

namespace vk1 {
LogicalDevice::LogicalDevice(const PhysicalDevice& physical_device, vk::SurfaceKHR surface)
    : physical_device_(physical_device), queue_family_info_(findQueueFamilyIndex(physical_device, surface)) {
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;
  for (uint32_t index : queue_family_info_.getIndices()) {
    vk::DeviceQueueCreateInfo queueCreateInfo{
        .queueFamilyIndex = index,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    queueCreateInfos.push_back(queueCreateInfo);
  }
  const auto& instance = getPhysicalDevice().getInstance();
  vk::PhysicalDeviceFeatures deviceFeatures;
  vk::DeviceCreateInfo createInfo{
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(instance.getEnabledlayers().size()),
      .ppEnabledLayerNames = instance.getEnabledlayers().data(),
      .enabledExtensionCount = static_cast<uint32_t>(physical_device.getEnabledExtensions().size()),
      .ppEnabledExtensionNames = physical_device.getEnabledExtensions().data(),
      .pEnabledFeatures = &deviceFeatures,
  };
  // create device
  vk_device_ = physical_device_.getVkPhysicalDevice().createDevice(createInfo);
  // get queue
  graphics_queue_ = vk_device_.getQueue(queue_family_info_.graphics_queue_family_index.value(), 0);
  present_queue_ = vk_device_.getQueue(queue_family_info_.present_queue_family_index.value(), 0);
  // create command pool
  command_pool_ =
      std::make_unique<CommandPool>(*this, queue_family_info_.graphics_queue_family_index.value());
  // create fence pool
}

LogicalDevice::~LogicalDevice() {
  command_pool_.reset();
  fence_pool_.reset();
  vk_device_.destroy();
}

QueueFamilyInfo LogicalDevice::findQueueFamilyIndex(const PhysicalDevice& physical_device,
                                                    vk::SurfaceKHR surface) {
  const auto& queueFamilyProperties = physical_device.getQueueFamilies();
  QueueFamilyInfo res{};
  uint32_t index = 0;
  for (const auto& queueFamily : queueFamilyProperties) {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
      res.graphics_queue_family_index = index;
    }
    auto presentSupport = physical_device.getVkPhysicalDevice().getSurfaceSupportKHR(index, surface);
    if (presentSupport) {
      res.present_queue_family_index = index;
    }
    if (res.graphics_queue_family_index.has_value() && res.present_queue_family_index.has_value()) {
      break;
    }
    index++;
  }
  if (!res.isComplete()) {
    throw std::runtime_error("cannot find suitable queue family indices!");
  }
  return res;
}

void LogicalDevice::createCommandPoolAndBuffers(uint32_t queue_family_index,
                                                uint32_t command_buffer_count,
                                                uint32_t in_flight_count) {
  if (!queue_family_info_.hasIndices(queue_family_index)) {
    throw std::runtime_error("create command pool failed, queue family index is not exist!");
  }
  commands_in_flight_ = in_flight_count;
  // TODO destroy previous command pool & buffers
  VkCommandPoolCreateInfo poolInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = queue_family_index,
  };
  if (vkCreateCommandPool(vk_device_, &poolInfo, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
  command_buffers_.resize(command_buffer_count);
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = command_pool_,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = command_buffer_count,
  };

  if (vkAllocateCommandBuffers(vk_device_, &allocInfo, command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
  for (uint32_t i = 0; i < commands_in_flight_; ++i) {
    VkFence fence;
    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    if (vkCreateFence(vk_device_, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
      throw std::runtime_error("create fence failed!");
    }
    fences_.push_back(std::move(fence));
  }
}

void LogicalDevice::createPipeline(const Pipeline::PipelineConfig& pipeline_config) {
  pipeline_ = std::make_unique<Pipeline>(*this, pipeline_config);
}

VkCommandBuffer LogicalDevice::getCommandBufferToBegin() {
  vkWaitForFences(vk_device_, 1, &fences_[current_fence_index_], VK_TRUE, UINT64_MAX);
  recordCommandBuffer();
  return command_buffers_[current_command_buffer_index_];
}

void LogicalDevice::recordCommandBuffer() {
  vkResetCommandBuffer(command_buffers_[current_command_buffer_index_],
                       VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = 0,
      .pInheritanceInfo = nullptr,
  };

  if (vkBeginCommandBuffer(command_buffers_[current_command_buffer_index_], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void LogicalDevice::endCommandBuffer(VkCommandBuffer command_buffer) {
  if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
    throw std::runtime_error("faield to record command buffer!");
  }
}

void LogicalDevice::submitCommand(VkCommandBuffer command_buffer, VkSubmitInfo submit_info) {
  vkResetFences(vk_device_, 1, &fences_[current_fence_index_]);
  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, fences_[current_fence_index_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
  current_command_buffer_index_ =
      (current_command_buffer_index_ + 1) % static_cast<uint32_t>(command_buffers_.size());
  current_fence_index_ = (current_fence_index_ + 1) % commands_in_flight_;
}

std::unique_ptr<Sampler> LogicalDevice::createSampler() {
  return std::make_unique<Sampler>(*this);
}
}  // namespace vk1
