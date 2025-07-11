// header
#define TINYGLTF_IMPLEMENTATION
#include "vk1/support/gltf_loader.hpp"
// library
#include <glm/gtc/type_ptr.hpp>
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

  const float* pos = nullptr;
  const float* colors = nullptr;
  const float* uvs = nullptr;
  // position
  auto& posAccessor = gltf_model_.accessors[gltfPrimitive.attributes.find("POSITION")->second];
  size_t vertexCount = posAccessor.count;
  auto& posBufferView = gltf_model_.bufferViews[posAccessor.bufferView];
  pos = reinterpret_cast<const float*>(
      &(gltf_model_.buffers[posBufferView.buffer].data[posAccessor.byteOffset + posBufferView.byteOffset]));
  model->vertices_count_ = static_cast<uint32_t>(vertexCount);
  for (size_t i = 0; i < vertexCount; ++i) {
    Vertex vert;
    vert.pos = glm::make_vec3(&pos[i * 3]);
  }
}
}  // namespace vk1
