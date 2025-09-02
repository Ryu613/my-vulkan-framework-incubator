#include "vk1/core/instance.hpp"

#include <map>
#include <vk1/common/util.hpp>

#include "vk1/core/physical_device.hpp"

namespace vk1 {
namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       vk::DebugUtilsMessageTypeFlagsEXT messageType,
                       const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";
  return VK_FALSE;
}
}  // namespace

Instance::Instance(std::string name,
                   OptionalLayers& required_layers,
                   OptionalExtensions& required_extensions,
                   bool is_debug)
    : name_(name), is_debug_(is_debug) {
  if (!checkLayerSupport(required_layers, is_debug) ||
      !checkInstanceExtensionSupport(required_extensions, is_debug)) {
    throw std::runtime_error("layers or extensions are not all supported!");
  }
  vk::ApplicationInfo appInfo{
      .pApplicationName = name_.c_str(),
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_3,
  };

  vk::InstanceCreateInfo createInfo{
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(enabled_layers_.size()),
      .ppEnabledLayerNames = enabled_layers_.data(),
      .enabledExtensionCount = static_cast<uint32_t>(enabled_extensions_.size()),
      .ppEnabledExtensionNames = enabled_extensions_.data(),
  };

  vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (is_debug_) {
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
  } else {
    createInfo.pNext = nullptr;
  }

  vk_instance_ = vk::createInstance(createInfo);
  if (is_debug_) {
    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo2;
    populateDebugMessengerCreateInfo(debugCreateInfo2);
    debug_utils_messenger_ = vk_instance_.createDebugUtilsMessengerEXT(debugCreateInfo2);
  }
  getAllPhysicalDevices();
}
Instance::~Instance() {
  if (is_debug_) {
    vk_instance_.destroyDebugUtilsMessengerEXT(debug_utils_messenger_);
  }
  vk_instance_.destroy();
}

bool Instance::checkLayerSupport(OptionalLayers& required_layers, bool is_debug) {
  if (required_layers.empty()) {
    if (!is_debug) {
      return true;
    }
    required_layers.insert(DEFAULT_DEBUG_LAYERS.begin(), DEFAULT_DEBUG_LAYERS.end());
  }
  std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
  // check if layers are all available & fill enabled_layers
  auto [supported, enabledNames] = util::isSupported<vk::LayerProperties>(
      availableLayers, required_layers, [](const vk::LayerProperties& p) { return p.layerName; });
  if (supported) {
    enabled_layers_.insert(enabled_layers_.end(), enabledNames.begin(), enabledNames.end());
  }
  return supported;
}

bool Instance::checkInstanceExtensionSupport(OptionalExtensions& required_extensions, bool is_debug) {
  if (required_extensions.empty()) {
    required_extensions.insert(DEFAULT_INSTANCE_EXTENSIONS.begin(), DEFAULT_INSTANCE_EXTENSIONS.end());
    if (is_debug) {
      required_extensions.insert(DEFAULT_DEBUG_INSTANCE_EXTENSIONS.begin(),
                                 DEFAULT_DEBUG_INSTANCE_EXTENSIONS.end());
    }
  }
  std::vector<vk::ExtensionProperties> extensions = vk::enumerateInstanceExtensionProperties();
  auto [supported, enabledNames] = util::isSupported<vk::ExtensionProperties>(
      extensions, required_extensions, [](const vk::ExtensionProperties& p) { return p.extensionName; });
  if (supported) {
    enabled_extensions_.insert(enabled_extensions_.end(), enabledNames.begin(), enabledNames.end());
  }
  return supported;
}

void Instance::populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info = {
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                     vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      .pfnUserCallback = debugMessengerCallback,
  };
}

void Instance::getAllPhysicalDevices() {
  std::vector<vk::PhysicalDevice> gpus = vk_instance_.enumeratePhysicalDevices();
  if (gpus.empty()) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  for (auto& each : gpus) {
    physical_devices_.push_back(std::make_unique<PhysicalDevice>(*this, each));
  }
}

const PhysicalDevice& Instance::chooseSuitablePhysicalDevice(VkSurfaceKHR surface,
                                                             OptionalExtensions& required_device_extensions) {
  if (required_device_extensions.empty()) {
    required_device_extensions.insert(DEFAULT_DEVICE_EXTENSIONS.begin(), DEFAULT_DEVICE_EXTENSIONS.end());
  }
  std::multimap<int, PhysicalDevice*> candidates;
  for (const auto& device : physical_devices_) {
    if (!device->supportExtensions(required_device_extensions)) {
      continue;
    }
    int score = rateDeviceSuitability(*device);
    candidates.insert({score, device.get()});
  }
  if (candidates.rbegin()->first > 0) {
    auto& chosenDevice = candidates.rbegin()->second;
    std::cout << "chosen device: " << chosenDevice->getProperties().deviceName << "\n";
    return *chosenDevice;
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

int Instance::rateDeviceSuitability(const PhysicalDevice& device) {
  const auto& device_properties = device.getProperties();
  const auto& device_features = device.getFeatures();
  int score = 0;
  if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
    score += 1000;
  }
  // Maximum possible size of textures affects graphics quality
  score += device_properties.limits.maxImageDimension2D;
  // Application can't function without geometry shaders
  if (!device_features.geometryShader) {
    return 0;
  }
  return score;
}
}  // namespace vk1
