#include "vk1/platform/glfw_window.hpp"

namespace vk1 {
GlfwWindow::GlfwWindow(const Properties& properties) : Window(properties) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(
      properties_.extent.width, properties_.extent.height, properties_.title.c_str(), nullptr, nullptr);
}
GlfwWindow::~GlfwWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool GlfwWindow::shouldClose() {
  return glfwWindowShouldClose(window_);
}
void GlfwWindow::processEvents() {
  glfwPollEvents();
}
void GlfwWindow::close() {
  glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

std::vector<const char*> GlfwWindow::getRequiredExtensions() const {
  uint32_t extensionsCount;
  const char** names = glfwGetRequiredInstanceExtensions(&extensionsCount);
  return {names, names + extensionsCount};
}
}  // namespace vk1
