#pragma once

#include "vk1/common/common.hpp"

namespace vk1 {
class Window {
 public:
  struct Extent {
    uint32_t width = 0;
    uint32_t height = 0;
  };
  enum class Mode {
    Headless,
    FullScreen,
    FullScreenBorderless,
    FullScreenStretch,
    Windowed,
  };
  struct Properties {
    std::string title{""};
    Mode mode = Mode::Windowed;
    bool resizable = true;
    Extent extent = {1024, 768};
  };
  MOVABLE_ONLY(Window);
  explicit Window(const Properties& properties) : properties_(properties) {}
  virtual ~Window() = default;

 public:
  virtual bool shouldClose() = 0;
  virtual void processEvents() {};
  virtual void close() = 0;
  virtual std::vector<const char*> getRequiredExtensions() const = 0;

  inline const Extent getExtent() const {
    return properties_.extent;
  }

  inline const Mode getMode() const {
    return properties_.mode;
  }

 protected:
  Properties properties_;
};
}  // namespace vk1
