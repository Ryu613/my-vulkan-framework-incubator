#pragma once

#include "vk1/common/common.hpp"

namespace vk1::util {
template <typename T>
using getTNameFunc = std::function<const char*(const T&)>;
struct SupportInfoBundle {
  bool isAllSupported = false;
  std::vector<const char*> enabled_names;
};

/**
 * @brief Checks whether the required extensions or layers are supported by the given list.
 *
 * This function is a generic utility to verify whether a set of required Vulkan extensions
 * or layers are available in a given list. It supports both mandatory and optional items.
 * A custom name extractor function can be passed to handle different Vulkan structures
 * like VkExtensionProperties or VkLayerProperties.
 *
 * @tparam T Type of the available items (e.g., VkExtensionProperties, VkLayerProperties)
 * @param available List of available Vulkan extensions or layers
 * @param required Map of required names and a flag indicating if they are optional (true = must)
 * @param getNameFunc A function that extracts the name (const char*) from an item of type T
 * @return SupportBundle Contains a flag indicating full support and a list of supported names
 */
template <typename T>
inline SupportInfoBundle isSupported(const std::vector<T>& available,
                                     const std::unordered_map<const char*, bool>& required,
                                     getTNameFunc<T> getNameFunc) {
  SupportInfoBundle result;
  for (const auto& [name, must] : required) {
    bool found = false;
    for (const auto& each : available) {
      if (strcmp(name, getNameFunc(each)) == 0) {
        result.enabled_names.push_back(name);
        found = true;
      }
    }
    if (!found && must) {
      return result;
    }
  }
  result.isAllSupported = true;
  return result;
}
}  // namespace vk1::util
