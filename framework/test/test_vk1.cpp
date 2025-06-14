#include "vk1/core/context.hpp"

int main() {
  vk1::ContextConfig config{
      .app_name = "default",
      .is_debug = true,
  };
  auto context = std::make_unique<vk1::Context>(config);
  // pseudo code for now
  // context.createSwapchain(...);
  // context.createShaderModule();
  // context.createRenderPass();
  // context.createGraphicsPipeline();
}
