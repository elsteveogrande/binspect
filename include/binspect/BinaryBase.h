#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/Section.h"
#include "binspect/Symbol.h"

namespace binspect {

struct BinaryBase {
  virtual Sections sections() const = 0;
  virtual Symbols symbols() const = 0;
};

}  // namespace binspect
