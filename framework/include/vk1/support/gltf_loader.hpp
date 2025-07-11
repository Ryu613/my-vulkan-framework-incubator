#pragma once

#include "vk1/core/common.hpp"
// tiny gltf
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

namespace vk1 {
class Model;
class GLTFLoader final {
 public:
  std::unique_ptr<Model> loadModel(const std::string& file_path);

 private:
  tinygltf::Model gltf_model_{};
  std::string model_path_{};
};
}  // namespace vk1
