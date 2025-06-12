#pragma once

#include "vk1/common/common.hpp"

namespace vk1 {
class PhysicalDevice;
class Instance {
 public:
  explicit Instance(std::string name,
                    const std::vector<const char*>& required_layers,
                    const std::vector<const char*>& required_extensions,
                    bool is_debug);
  ~Instance();

  inline VkInstance& getVkInstance() {
    return vk_instance_;
  }

  PhysicalDevice& choosePhysicalDevice();

  NO_COPY_MOVE(Instance);

 private:
  std::string name_;
  VkInstance vk_instance_;
  std::vector<const char*> enabled_extensions_;
  VkDebugUtilsMessengerEXT debug_utils_messenger_;
  std::vector<std::unique_ptr<PhysicalDevice>> physical_devices_;

  bool checkLayerSupport(const std::vector<const char*>& required_layers);
  bool checkInstanceExtensionSupport(const std::vector<const char*>& required_extensions);
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void getAllPhysicalDevices();
};
}  // namespace vk1
