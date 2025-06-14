#include "vk1/core/instance.hpp"

#include <map>
#include <vk1/common/util.hpp>

#include "vk1/core/physical_device.hpp"

namespace vk1 {
namespace {
VkResult createDebugutilsMessengerExt(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}
VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
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
  if (volkInitialize()) {
    throw std::runtime_error("failed to initialize volk!");
  }
  if (!checkLayerSupport(required_layers, is_debug) ||
      !checkInstanceExtensionSupport(required_extensions, is_debug)) {
    throw std::runtime_error("layers or extensions are not all supported!");
  }
  VkApplicationInfo appInfo{
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pApplicationName = name_.c_str(),
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = VK_API_VERSION_1_0,
  };

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(enabled_layers_.size()),
      .ppEnabledLayerNames = enabled_layers_.data(),
      .enabledExtensionCount = static_cast<uint32_t>(enabled_extensions_.size()),
      .ppEnabledExtensionNames = enabled_extensions_.data(),
  };

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (is_debug_) {
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
  } else {
    createInfo.pNext = nullptr;
  }
  if (vkCreateInstance(&createInfo, nullptr, &vk_instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
  if (is_debug_) {
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo2;
    populateDebugMessengerCreateInfo(debugCreateInfo2);
    if (createDebugutilsMessengerExt(vk_instance_, &debugCreateInfo2, nullptr, &debug_utils_messenger_) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to setup debug messenger!");
    }
  }
  volkLoadInstance(vk_instance_);
  getAllPhysicalDevices();
}
Instance::~Instance() {
  if (is_debug_) {
    vkDestroyDebugUtilsMessengerEXT(vk_instance_, debug_utils_messenger_, nullptr);
  }
  vkDestroyInstance(vk_instance_, nullptr);
}

bool Instance::checkLayerSupport(OptionalLayers& required_layers, bool is_debug) {
  if (required_layers.empty()) {
    if (!is_debug) {
      return true;
    }
    required_layers.insert(DEFAULT_DEBUG_LAYERS.begin(), DEFAULT_DEBUG_LAYERS.end());
  }
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  // check if layers are all available & fill enabled_layers
  auto [supported, enabledNames] = util::isSupported<VkLayerProperties>(
      availableLayers, required_layers, [](const VkLayerProperties& p) { return p.layerName; });
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
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  auto [supported, enabledNames] = util::isSupported<VkExtensionProperties>(
      extensions, required_extensions, [](const VkExtensionProperties& p) { return p.extensionName; });
  if (supported) {
    enabled_extensions_.insert(enabled_extensions_.end(), enabledNames.begin(), enabledNames.end());
  }
  return supported;
}

void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) {
  create_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugMessengerCallback,
  };
}

void Instance::getAllPhysicalDevices() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vk_instance_, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vk_instance_, &deviceCount, devices.data());

  for (auto& each : devices) {
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
    return *(candidates.rbegin()->second);
  } else {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

int Instance::rateDeviceSuitability(const PhysicalDevice& device) {
  const auto& device_properties = device.getProperties();
  const auto& device_features = device.getFeatures();
  int score = 0;
  if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
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
