#pragma once

#include "vk1/core/common.hpp"
#include "vk1/core/physical_device.hpp"
#include "vk1/core/pipeline.hpp"

namespace vk1 {
class Swapchain;
struct QueueFamilyInfo {
  std::optional<uint32_t> graphics_queue_family_index;
  std::optional<uint32_t> present_queue_family_index;
  bool hasIndices(uint32_t index) const {
    if (index != graphics_queue_family_index.value() && index != present_queue_family_index.value()) {
      return false;
    }
    return true;
  }
  std::set<uint32_t> getIndices() const {
    if (isComplete()) {
      return {graphics_queue_family_index.value(), present_queue_family_index.value()};
    }
    return {};
  }
  bool isComplete() const {
    return graphics_queue_family_index.has_value() && present_queue_family_index.has_value();
  }
};

class LogicalDevice final {
 public:
  NO_COPY_MOVE(LogicalDevice);

  explicit LogicalDevice(const PhysicalDevice& physical_device, VkSurfaceKHR surface);

  ~LogicalDevice();

 public:
  inline const PhysicalDevice& getPhysicalDevice() const {
    return physical_device_;
  }

  inline const VkDevice& getVkDevice() const {
    return vk_device_;
  }

  inline const QueueFamilyInfo& getQueueFamilyInfo() const {
    return queue_family_info_;
  }

  inline VkFence getCurrentFence() const {
    return fences_[current_fence_index_];
  }

  inline const Pipeline& getPipeline() const {
    return *pipeline_;
  }

  inline VkQueue getPresentQueue() const {
    return present_queue_;
  }

  void createCommandPoolAndBuffers(uint32_t queue_family_index,
                                   uint32_t command_buffer_count,
                                   uint32_t in_flight_count = 2);

  void createPipeline(const Pipeline::PipelineConfig& pipeline_config);

  VkCommandBuffer getCommandBufferToBegin();

  void endCommandBuffer(VkCommandBuffer command_buffer);

  void submitCommand(VkCommandBuffer command_buffer, VkSubmitInfo submit_info);

 private:
  const PhysicalDevice& physical_device_;
  VkDevice vk_device_;
  QueueFamilyInfo queue_family_info_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;
  VkCommandPool command_pool_;
  std::vector<VkCommandBuffer> command_buffers_;
  std::unique_ptr<Pipeline> pipeline_;
  std::vector<VkFence> fences_;
  uint32_t current_fence_index_ = 0;
  uint32_t current_command_buffer_index_ = 0;
  uint32_t commands_in_flight_ = 2;

 private:
  QueueFamilyInfo findQueueFamilyIndex(const PhysicalDevice& physical_device, VkSurfaceKHR surface);
  void recordCommandBuffer();
};

}  // namespace vk1
