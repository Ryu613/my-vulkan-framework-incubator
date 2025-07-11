#pragma once

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
  uint32_t vertices_count_{0};
};
}  // namespace vk1
