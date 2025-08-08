#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class LogicalDevice;
class Swapchain;
class Window;
class FrameContext;
class RenderContext final {
 public:
  struct Config {
    vk::SurfaceKHR surface;
    vk::SurfaceFormatKHR surface_format = vk::Format::eR8G8B8A8Srgb;
    vk::ColorSpaceKHR color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;
  };

 public:
  NO_COPY_MOVE(RenderContext);

  RenderContext(const LogicalDevice& device, const Window& window, RenderContext::Config config);

  ~RenderContext();

 private:
  void prepareFrames();

 private:
  const LogicalDevice& logical_device_;
  const Window& window_;
  RenderContext::Config config_;
  std::unique_ptr<Swapchain> swapchain_;
  std::vector<std::unique_ptr<FrameContext>> frames_;
};
}  // namespace vk1
