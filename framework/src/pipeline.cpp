#include "vk1/core/pipeline.hpp"

#include "vk1/core/logical_device.hpp"
#include "ze/utils/filesystem.hpp"

namespace vk1 {
Pipeline::Pipeline(const LogicalDevice& logical_device, Pipeline::Config config)
    : logical_device_(logical_device), config_(config) {
  switch (config.pipelineType) {
    case Pipeline::Type::GraphicsPipeline:
      createDescriptorSetLayout();
      createGraphicsPipeline();
      break;
    default:
      throw std::runtime_error("cannot create pipeline!");
  }
}

Pipeline::~Pipeline() {
  auto device = logical_device_.getVkDevice();
  device.destroyPipeline(vk_pipeline_);
  device.destroyPipelineLayout(vk_pipeline_layout_);
}

void Pipeline::createDescriptorSetLayout() {
  vk::DescriptorSetLayoutBinding uboLayoutBinding{
      .binding = 0,
      .descriptorType = vk::DescriptorType::eUniformBuffer,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eVertex,
      .pImmutableSamplers = nullptr,
  };

  vk::DescriptorSetLayoutBinding samplerLayoutBinding{
      .binding = 1,
      .descriptorType = vk::DescriptorType::eCombinedImageSampler,
      .descriptorCount = 1,
      .stageFlags = vk::ShaderStageFlagBits::eFragment,
      .pImmutableSamplers = nullptr,
  };

  std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

  vk::DescriptorSetLayoutCreateInfo layoutInfo{
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data(),
  };

  vk_descriptor_set_layout_ = logical_device_.getVkDevice().createDescriptorSetLayout(layoutInfo);
}

void Pipeline::createGraphicsPipeline() {
  auto& fsPtr = *(ze::utils::filesystem::get());

  auto vertShaderCode =
      fsPtr.read_file_string(static_cast<std::filesystem::path>(pipeline_config_.vertexShaderPath));
  auto fragShaderCode =
      fsPtr.read_file_string(static_cast<std::filesystem::path>(pipeline_config_.fragmentShaderPath));

  vk::ShaderModule vertShaderModule = createShaderModule(std::move(vertShaderCode));
  vk::ShaderModule fragShaderModule = createShaderModule(std::move(fragShaderCode));

  vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eVertex,
      .module = vertShaderModule,
      .pName = "main",
  };

  vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eFragment,
      .module = fragShaderModule,
      .pName = "main",
  };

  vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
      .vertexBindingDescriptionCount = 0,
      .pVertexBindingDescriptions = nullptr,
      .vertexAttributeDescriptionCount = 0,
      .pVertexAttributeDescriptions = nullptr,
  };

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
      .topology = vk::PrimitiveTopology::eTriangleList,
      .primitiveRestartEnable = VK_FALSE,
  };

  vk::Viewport viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(pipeline_config_.viewportExtent.width),
      .height = static_cast<float>(pipeline_config_.viewportExtent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  };

  vk::Rect2D scissor{
      .offset = {0, 0},
      .extent = pipeline_config_.viewportExtent,
  };

  std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamicState{
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data(),
  };

  vk::PipelineViewportStateCreateInfo viewportState{
      .viewportCount = 1,
      .scissorCount = 1,
  };

  vk::PipelineRasterizationStateCreateInfo rasterizer{
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = vk::PolygonMode::eFill,
      .cullMode = vk::CullModeFlagBits::eBack,
      .frontFace = vk::FrontFace::eClockwise,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f,
  };

  vk::PipelineMultisampleStateCreateInfo multisampling{
      .rasterizationSamples = vk::SampleCountFlagBits::e1,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE,
  };

  vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = VK_FALSE,
      .srcColorBlendFactor = vk::BlendFactor::eOne,
      .dstColorBlendFactor = vk::BlendFactor::eZero,
      .colorBlendOp = vk::BlendOp::eAdd,
      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
      .alphaBlendOp = vk::BlendOp::eAdd,
      .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
  };

  vk::PipelineColorBlendStateCreateInfo colorBlending{
      .logicOpEnable = VK_FALSE,
      .logicOp = vk::LogicOp::eCopy,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment,
  };
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  vk_pipeline_layout_ = logical_device_.getVkDevice().createPipelineLayout(pipelineLayoutInfo);

  vk::GraphicsPipelineCreateInfo pipelineInfo{
      .stageCount = 2,
      .pStages = shaderStages,

      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &inputAssembly,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizer,
      .pMultisampleState = &multisampling,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &colorBlending,
      .pDynamicState = &dynamicState,
      .layout = vk_pipeline_layout_,
      .renderPass = pipeline_config_.renderPass,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };

  vk_pipeline_ = logical_device_.getVkDevice().createGraphicsPipeline(pipelineInfo);

  logical_device_.getVkDevice().destroyShaderModule(fragShaderModule);
  logical_device_.getVkDevice().destroyShaderModule(vertShaderModule);
}

vk::ShaderModule Pipeline::createShaderModule(std::string&& shader_code) {
  vk::ShaderModuleCreateInfo createInfo{
      .codeSize = shader_code.length(),
      .pCode = reinterpret_cast<const uint32_t*>(shader_code.data()),
  };
  vk::ShaderModule shaderModule = logical_device_.getVkDevice().createShaderModule(createInfo);
  return shaderModule;
}
}  // namespace vk1
