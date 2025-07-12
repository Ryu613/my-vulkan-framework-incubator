#pragma once

#include "vk1/core/buffer.hpp"
#include "vk1/core/common.hpp"

namespace vk1 {
struct Vertex {
  glm::vec3 pos{};
  glm::vec3 color{};
  glm::vec2 texCoord{};
};

struct Mesh {
  using indices_t = uint32_t;
  std::vector<Vertex> vertices{};
  std::vector<indices_t> indices{};
};

struct Model {
  std::vector<Mesh> meshes_;
  std::vector<uint32_t> indices_;
  uint32_t vertices_count_{0};
  uint32_t vertex_indices_count_{0};
  std::unique_ptr<Buffer> vertex_buffer_;
  std::unique_ptr<Buffer> index_buffer_;
};
}  // namespace vk1
