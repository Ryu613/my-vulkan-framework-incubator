#include "vk1/core/instance.hpp"

namespace vk1 {
namespace {
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";
  return VK_FALSE;
}
}  // namespace
Instance::Instance(std::string name,
                   const std::vector<const char*>& required_layers,
                   const std::vector<const char*>& required_extensions,
                   bool is_debug)
    : name_(name) {
  if (!checkLayerSupport(required_layers)) {
    throw std::runtime_error("validation layers requested, but not available!");
  }
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = name_.c_str();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  checkInstanceExtensionSupport(required_extensions);

  createInfo.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
  createInfo.ppEnabledExtensionNames = required_extensions.data();

  createInfo.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
  createInfo.ppEnabledLayerNames = required_layers.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  createInfo.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
  createInfo.ppEnabledLayerNames = required_layers.data();
  if (is_debug) {
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
  } else {
    createInfo.pNext = nullptr;
  }
  if (vkCreateInstance(&createInfo, nullptr, &vk_instance_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
  if (is_debug) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (vkCreateDebugUtilsMessengerEXT(vk_instance_, &debugCreateInfo, nullptr, &debug_utils_messenger_) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to setup debug messenger!");
    }
  }
  getAllPhysicalDevices();
}
Instance::~Instance() {}

bool Instance::checkLayerSupport(const std::vector<const char*>& required_layers) {
  if (required_layers.empty()) {
    return false;
  }
  // get available layers
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  // check if validationLayers are all available
  bool layerFound = false;
  for (const auto& layerName : required_layers) {
    layerFound = false;
    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) {
      return false;
    }
  }
  return true;
}

bool Instance::checkInstanceExtensionSupport(const std::vector<const char*>& required_extensions) {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
  bool isAllExtensionsSupported = false;
  for (int i = 0; i < required_extensions.size(); ++i) {
    isAllExtensionsSupported = false;
    for (const auto& each : extensions) {
      if (strcmp(each.extensionName, required_extensions[i]) == 0) {
        isAllExtensionsSupported = true;
        break;
      }
    }
    if (!isAllExtensionsSupported) {
      throw std::runtime_error("extensions not supported: " + std::string(required_extensions[i]));
    }
  }
}

void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
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
}  // namespace vk1
