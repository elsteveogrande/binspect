#pragma once

#include "binspect/_config.h"
#include "binspect/memory.h"

namespace binspect {

struct binary;

struct context {
  res<binary> binary_at(void const* ptr);
};

}  // namespace binspect
