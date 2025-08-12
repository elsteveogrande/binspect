#pragma once

#include "binspect/_config.h"
#include "binspect/memory.h"

namespace binspect {

struct binary;

struct context {
  heap heap_;

  explicit context(std::pmr::memory_resource& rs) : heap_(rs) {}

  res<binary> binary_at(void const* ptr);
};

}  // namespace binspect
