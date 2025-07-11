// header
#define TINYGLTF_IMPLEMENTATION
#include "vk1/support/gltf_loader.hpp"
// local
#include "vk1/support/model.hpp"

namespace vk1 {
std::unique_ptr<Model> GLTFLoader::loadModel(const std::string& file_path) {
  std::string err;
  std::string warn;

  tinygltf::TinyGLTF gltf_loader;

  bool importResult = gltf_loader.LoadASCIIFromFile(&gltf_model_, &err, &warn, file_path.c_str());

  if (!importResult) {
    throw std::runtime_error("failed to load model!");
  }
  if (!err.empty()) {
    throw std::runtime_error("failed to load model due to " + err);
  }
  auto model = std::make_unique<Model>();

  auto& gltfMesh = gltf_model_.meshes[0];
  auto& gltfPrimitive = gltfMesh.primitives[0];

  std::vector<Vertex> vertexData;
}
}  // namespace vk1
