// header
#define TINYGLTF_IMPLEMENTATION
#include "vk1/support/gltf_loader.hpp"
// library
#include <glm/gtc/type_ptr.hpp>
// local
#include "vk1/common/util.hpp"
#include "vk1/support/model.hpp"

namespace vk1 {
namespace {
inline VkFormat getAttributeFormat(const tinygltf::Model* model, uint32_t accessorId) {
  assert(accessorId < model->accessors.size());
  auto& accessor = model->accessors[accessorId];

  VkFormat format;

  switch (accessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE: {
      static const std::map<int, VkFormat> mapped_format = {{TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_SINT},
                                                            {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_SINT},
                                                            {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_SINT},
                                                            {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_SINT}};

      format = mapped_format.at(accessor.type);

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
      static const std::map<int, VkFormat> mapped_format = {{TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_UINT},
                                                            {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_UINT},
                                                            {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_UINT},
                                                            {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_UINT}};

      static const std::map<int, VkFormat> mapped_format_normalize = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_UNORM},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_UNORM},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_UNORM},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_UNORM}};

      if (accessor.normalized) {
        format = mapped_format_normalize.at(accessor.type);
      } else {
        format = mapped_format.at(accessor.type);
      }

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_SHORT: {
      static const std::map<int, VkFormat> mapped_format = {{TINYGLTF_TYPE_SCALAR, VK_FORMAT_R8_SINT},
                                                            {TINYGLTF_TYPE_VEC2, VK_FORMAT_R8G8_SINT},
                                                            {TINYGLTF_TYPE_VEC3, VK_FORMAT_R8G8B8_SINT},
                                                            {TINYGLTF_TYPE_VEC4, VK_FORMAT_R8G8B8A8_SINT}};

      format = mapped_format.at(accessor.type);

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
      static const std::map<int, VkFormat> mapped_format = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R16_UINT},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R16G16_UINT},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R16G16B16_UINT},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R16G16B16A16_UINT}};

      static const std::map<int, VkFormat> mapped_format_normalize = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R16_UNORM},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R16G16_UNORM},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R16G16B16_UNORM},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R16G16B16A16_UNORM}};

      if (accessor.normalized) {
        format = mapped_format_normalize.at(accessor.type);
      } else {
        format = mapped_format.at(accessor.type);
      }

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_INT: {
      static const std::map<int, VkFormat> mapped_format = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_SINT},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_SINT},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_SINT},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_SINT}};

      format = mapped_format.at(accessor.type);

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
      static const std::map<int, VkFormat> mapped_format = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_UINT},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_UINT},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_UINT},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_UINT}};

      format = mapped_format.at(accessor.type);

      break;
    }
    case TINYGLTF_COMPONENT_TYPE_FLOAT: {
      static const std::map<int, VkFormat> mapped_format = {
          {TINYGLTF_TYPE_SCALAR, VK_FORMAT_R32_SFLOAT},
          {TINYGLTF_TYPE_VEC2, VK_FORMAT_R32G32_SFLOAT},
          {TINYGLTF_TYPE_VEC3, VK_FORMAT_R32G32B32_SFLOAT},
          {TINYGLTF_TYPE_VEC4, VK_FORMAT_R32G32B32A32_SFLOAT}};

      format = mapped_format.at(accessor.type);

      break;
    }
    default: {
      format = VK_FORMAT_UNDEFINED;
      break;
    }
  }

  return format;
};

inline std::vector<uint8_t> getAttributeData(const tinygltf::Model* model, uint32_t accessorId) {
  assert(accessorId < model->accessors.size());
  auto& accessor = model->accessors[accessorId];
  assert(accessor.bufferView < model->bufferViews.size());
  auto& bufferView = model->bufferViews[accessor.bufferView];
  assert(bufferView.buffer < model->buffers.size());
  auto& buffer = model->buffers[bufferView.buffer];

  size_t stride = accessor.ByteStride(bufferView);
  size_t startByte = accessor.byteOffset + bufferView.byteOffset;
  size_t endByte = startByte + accessor.count * stride;

  return {buffer.data.begin() + startByte, buffer.data.begin() + endByte};
};

inline std::vector<uint8_t> convertUnderlyingDataStride(const std::vector<uint8_t>& src_data,
                                                        uint32_t src_stride,
                                                        uint32_t dst_stride) {
  auto elem_count = util::castU32(src_data.size()) / src_stride;

  std::vector<uint8_t> result(elem_count * dst_stride);

  for (uint32_t idxSrc = 0, idxDst = 0; idxSrc < src_data.size() && idxDst < result.size();
       idxSrc += src_stride, idxDst += dst_stride) {
    std::copy(src_data.begin() + idxSrc, src_data.begin() + idxSrc + src_stride, result.begin() + idxDst);
  }

  return result;
}

}  // namespace
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
  uint32_t colorComponentCount{3};
  // position
  auto& accessor = gltf_model_.accessors[gltfPrimitive.attributes.find("POSITION")->second];
  size_t vertexCount = accessor.count;
  auto& bufferView = gltf_model_.bufferViews[accessor.bufferView];
  pos = reinterpret_cast<const float*>(
      &(gltf_model_.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
  model->vertices_count_ = static_cast<uint32_t>(vertexCount);
  // color
  if (gltfPrimitive.attributes.find("COLOR_0") != gltfPrimitive.attributes.end()) {
    accessor = gltf_model_.accessors[gltfPrimitive.attributes.find("COLOR_0")->second];
    bufferView = gltf_model_.bufferViews[accessor.bufferView];
    colors = reinterpret_cast<const float*>(
        &(gltf_model_.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
  }
  // texture uv
  if (gltfPrimitive.attributes.find("TEXCOORD_0") != gltfPrimitive.attributes.end()) {
    accessor = gltf_model_.accessors[gltfPrimitive.attributes.find("TEXCOORD_0")->second];
    bufferView = gltf_model_.bufferViews[accessor.bufferView];
    uvs = reinterpret_cast<const float*>(
        &(gltf_model_.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]));
  }
  for (size_t i = 0; i < vertexCount; ++i) {
    Vertex vert;
    vert.pos = glm::make_vec3(&pos[i * 3]);
    vert.texCoord = uvs ? glm::make_vec2(&colors[i * 2]) : glm::vec3(0.0f);
    if (colors) {
      switch (colorComponentCount) {
        case 3:
          vert.color = glm::make_vec3(&colors[i * 3]);
        case 4:
          vert.color = glm::make_vec4(&colors[i * 4]);
      }
    } else {
      vert.color = glm::vec3(1.0f);
    }
    vertexData.push_back(vert);
  }
  // vertex indices
  if (gltfPrimitive.indices >= 0) {
    model->vertex_indices_count_ = util::castU32(gltf_model_.accessors[gltfPrimitive.indices].count);

    auto indicesFormat = getAttributeFormat(&gltf_model_, gltfPrimitive.indices);
    auto indicesData = getAttributeData(&gltf_model_, gltfPrimitive.indices);

    switch (indicesFormat) {
      case VK_FORMAT_R32_UINT: {
        // Correct format
        break;
      }
      case VK_FORMAT_R16_UINT: {
        indicesData = convertUnderlyingDataStride(indicesData, 2, 4);
        break;
      }
      case VK_FORMAT_R8_UINT: {
        indicesData = convertUnderlyingDataStride(indicesData, 1, 4);
        break;
      }
      default: {
        break;
      }
    }

    model->indices_.resize(indicesData.size() / sizeof(uint32_t));
    std::memcpy(model->indices_.data(), indicesData.data(), indicesData.size());
  }
}
}  // namespace vk1
