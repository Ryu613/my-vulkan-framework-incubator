#include "vk1/core/context.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

GLFWwindow* window = nullptr;

vk1::OptionalExtensions getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // fill vector with glfw extensions(by pointer start to end)
  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  vk1::OptionalExtensions ext;
  for (const auto& each : extensions) {
    ext.insert({each, true});
  }
  return ext;
}
int main() {
  // init glfw
  glfwInit();
  // tell GLFW not to create OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(1024, 768, "Vulkan", nullptr, nullptr);
  // glfwSetWindowUserPointer(window, this);
  // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  vk1::ContextConfig config{
      .app_name = "default",
      .window = window,
      .is_debug = true,
      .required_layers = {{"VK_LAYER_KHRONOS_validation", true}},
      .required_instance_extensions = getRequiredExtensions(),
  };
  auto context = std::make_unique<vk1::Context>(config);
}
// pseudo code for now
// context.createSwapchain(...);
// context.createShaderModule();
// context.createRenderPass();
// context.createGraphicsPipeline();
