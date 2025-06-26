#include "vk1/core/context.hpp"
#include "vk1/platform/glfw_window.hpp"

using namespace vk1;

std::unique_ptr<Window> window = nullptr;

OptionalExtensions getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // fill vector with glfw extensions(by pointer start to end)
  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  OptionalExtensions ext;
  for (const auto& each : extensions) {
    ext.insert({each, true});
  }
  return ext;
}

int main() {
  Window::Properties windowProps{
      .title = "test",
      .mode = Window::Mode::Windowed,
      .resizable = true,
      .extent = {1024, 768},
  };
  window = std::make_unique<GlfwWindow>(windowProps);
  // Window window = glfwCreateWindow(1024, 768, "Vulkan", nullptr, nullptr);
  // glfwSetWindowUserPointer(window, this);
  // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  ContextConfig config{
      .app_name = "default",
      .window = window.get(),
      .is_debug = true,
      .required_layers = {{"VK_LAYER_KHRONOS_validation", true}},
      .required_instance_extensions = getRequiredExtensions(),
  };
  auto context = std::make_unique<Context>(config);
  while (!window->shouldClose()) {
    window->processEvents();
    context->drawFrame();
  }
  context.reset();
  window.reset();
}
