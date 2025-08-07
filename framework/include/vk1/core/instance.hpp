#pragma once

#include "vk1/core/common.hpp"

namespace vk1 {
class PhysicalDevice;
class Instance final {
 public:
  NO_COPY_MOVE(Instance);

  explicit Instance(std::string name,
                    OptionalLayers& required_layers,
                    OptionalExtensions& required_instance_extensions,
                    bool is_debug);
  ~Instance();

 public:
  inline const vk::Instance& getVkInstance() const {
    return vk_instance_;
  }

  inline const std::vector<const char*>& getEnabledlayers() const {
    return enabled_layers_;
  }

  inline const std::vector<const char*>& getEnabledExtensions() const {
    return enabled_extensions_;
  }

  const PhysicalDevice& chooseSuitablePhysicalDevice(VkSurfaceKHR surface,
                                                     OptionalExtensions& reuiqred_device_extensions);

 private:
  std::string name_;
  bool is_debug_ = false;
  vk::Instance vk_instance_;
  std::vector<const char*> enabled_extensions_;
  std::vector<const char*> enabled_layers_;
  vk::DebugUtilsMessengerEXT debug_utils_messenger_;
  std::vector<std::unique_ptr<PhysicalDevice>> physical_devices_;

 private:
  bool checkLayerSupport(OptionalLayers& required_layers, bool is_debug);
  bool checkInstanceExtensionSupport(OptionalExtensions& required_extensions, bool is_debug);
  void populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);
  void getAllPhysicalDevices();

 private:
  int rateDeviceSuitability(const PhysicalDevice& device);
};
}  // namespace vk1
