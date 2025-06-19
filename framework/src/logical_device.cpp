#include "vk1/core/logical_device.hpp"

namespace vk1 {
LogicalDevice::LogicalDevice(const PhysicalDevice& physical_device, VkSurfaceKHR surface)
    : physical_device_(physical_device), queue_family_info_(findQueueFamilyIndex(physical_device, surface)) {
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : queue_family_info_.getIndices()) {
    VkDeviceQueueCreateInfo queueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };
    queueCreateInfos.push_back(queueCreateInfo);
  }
  const auto& instance = getPhysicalDevice().getInstance();
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = static_cast<uint32_t>(instance.getEnabledlayers().size()),
      .ppEnabledLayerNames = instance.getEnabledlayers().data(),
      .enabledExtensionCount = static_cast<uint32_t>(physical_device.getEnabledExtensions().size()),
      .ppEnabledExtensionNames = physical_device.getEnabledExtensions().data(),
      .pEnabledFeatures = &deviceFeatures,
  };

  if (vkCreateDevice(physical_device.getVkPhysicalDevice(), &createInfo, nullptr, &vk_device_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }
  volkLoadDevice(vk_device_);
  // vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  // vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

LogicalDevice::~LogicalDevice() {}

QueueFamilyInfo LogicalDevice::findQueueFamilyIndex(const PhysicalDevice& physical_device,
                                                    VkSurfaceKHR surface) {
  const auto& queueFamilyProperties = physical_device.getQueueFamilies();
  QueueFamilyInfo res{};
  uint32_t index = 0;
  for (const auto& queueFamily : queueFamilyProperties) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      res.graphics_queue_family_index = index;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        physical_device.getVkPhysicalDevice(), index, surface, &presentSupport);
    if (presentSupport) {
      res.present_queue_family_index = index;
    }
    if (res.graphics_queue_family_index.has_value() && res.present_queue_family_index.has_value()) {
      break;
    }
    index++;
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
    VkFenceCreateInfo fenceInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    if (vkCreateFence(vk_device_, &fenceInfo, nullptr, &fences_[i]) != VK_SUCCESS) {
      throw std::runtime_error("create fence failed!");
    }
  }
}

void LogicalDevice::createPipeline(const Pipeline::PipelineConfig& pipeline_config) {
  pipeline_ = std::make_unique<Pipeline>(*this, pipeline_config);
}

// VkCommandBuffer LogicalDevice::getCommandBufferToBegin() {
//   vkWaitForFences(vk_device_, 1, &fences_[current_fence_index_], VK_TRUE, UINT64_MAX);
// }
}  // namespace vk1
