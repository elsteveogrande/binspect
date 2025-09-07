#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/Gen.h"
#include "binspect/Section.h"
// #include "binspect/Symbol.h"

namespace binspect {

struct BinaryBase {
  virtual Gen<Section> sections() const = 0;
  virtual Section section(uint16_t i) const = 0;

  // virtual View<Symbol> symbols() const = 0;
};

}  // namespace binspect
