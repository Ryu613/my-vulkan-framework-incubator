#pragma once

#include "core/common.hpp"

namespace vk1 {
class Context;
class Buffer final {
 private:
  const Context& context_;
  VmaAllocator allocator_;
}
}  // namespace vk1
