#include "vk1/context.hpp"

int main() {
  vk1::ContextConfig config;
  vk1::Context context(config);
  // pseudo code for now
  context.createSwapchain(...);
  context.createShaderModule();
  context.createRenderPass();
  context.createGraphicsPipeline();
}
