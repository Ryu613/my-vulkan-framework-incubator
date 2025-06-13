#pragma once

#include "vk1/common/common.hpp"

namespace vk1 {
class PhysicalDevice;
class Instance {
 public:
  NO_COPY_MOVE(Instance);

  explicit Instance(std::string name,
                    const OptionalLayers& required_layers,
                    const OptionalExtensions& required_instance_extensions,
                    bool is_debug);
  ~Instance();

 public:
  inline const VkInstance& getVkInstance() const {
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
  VkInstance vk_instance_;
  std::vector<const char*> enabled_extensions_;
  std::vector<const char*> enabled_layers_;
  VkDebugUtilsMessengerEXT debug_utils_messenger_;
  std::vector<std::unique_ptr<PhysicalDevice>> physical_devices_;

 private:
  bool checkLayerSupport(const OptionalLayers& required_layers);
  bool checkInstanceExtensionSupport(const OptionalExtensions& required_extensions);
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);
  void getAllPhysicalDevices();
  int rateDeviceSuitability(const PhysicalDevice& device);
};
}  // namespace vk1
