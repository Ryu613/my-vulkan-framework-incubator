#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Pipeline final {
 public:
  enum class PipelineType { GraphicsPipeline, ComputePipeline, RayTracingPipeline };
  struct PipelineConfig {
    PipelineType pipelineType = PipelineType::GraphicsPipeline;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    VkRenderPass renderPass;
    VkExtent2D viewportExtent;
  };

 public:
  NO_COPY_MOVE(Pipeline);

  explicit Pipeline(const LogicalDevice& logical_device, const PipelineConfig& pipeline_config);

  ~Pipeline();

 public:
  inline const VkPipeline getVkPipeline() const {
    return vk_pipeline_;
  }

 private:
  const LogicalDevice& logical_device_;
  PipelineConfig pipeline_config_;
  VkPipeline vk_pipeline_{VK_NULL_HANDLE};
  VkPipelineLayout vk_pipeline_layout_{VK_NULL_HANDLE};

  VkDescriptorSetLayout vk_descriptor_set_layout_{VK_NULL_HANDLE};

 private:
  void createGraphicsPipeline();
  void createDescriptorSetLayout();
  VkShaderModule createShaderModule(std::string&& shader_code);
};
}  // namespace vk1
