#pragma once

#include "vk1/platform/window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

struct GLFWwindow;

namespace vk1 {
class GlfwWindow : public Window {
 public:
  explicit GlfwWindow(const Properties& properties);
  ~GlfwWindow();

 public:
  bool shouldClose() override;
  void processEvents() override;
  void close() override;
  std::vector<const char*> getRequiredExtensions() const override;

 private:
  GLFWwindow* window_ = nullptr;
};
}  // namespace vk1
