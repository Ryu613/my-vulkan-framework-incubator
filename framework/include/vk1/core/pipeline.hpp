#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Pipeline final {
 public:
  enum class Type { GraphicsPipeline, ComputePipeline, RayTracingPipeline };
  struct Config {
    Pipeline::Type pipelineType = Pipeline::Type::GraphicsPipeline;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    vk::RenderPass renderPass;
    vk::Extent2D viewportExtent;
  };

 public:
  NO_COPY_MOVE(Pipeline);

  explicit Pipeline(const LogicalDevice& logical_device, Pipeline::Config config);

  ~Pipeline();

 public:
  inline const VkPipeline getVkPipeline() const {
    return vk_pipeline_;
  }

 private:
  const LogicalDevice& logical_device_;
  Pipeline::Config config_;
  vk::Pipeline vk_pipeline_;
  vk::PipelineLayout vk_pipeline_layout_;

  vk::DescriptorSetLayout vk_descriptor_set_layout_;

 private:
  void createGraphicsPipeline();
  void createDescriptorSetLayout();
  vk::ShaderModule createShaderModule(std::string&& shader_code);
};
}  // namespace vk1
