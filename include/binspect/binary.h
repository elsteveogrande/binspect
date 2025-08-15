#pragma once

#include "binspect/section.h"
#include "binspect/symbol.h"
#include "binspect/view.h"

#include <format>
#include <ostream>
#include <print>

namespace binspect {

struct binary final {
  std::function<view<section>()> sections {};
  std::function<view<symbol>()> symbols {};

  friend std::ostream& operator<<(std::ostream& os, binary const& self) {
    std::print(os, "(binary sectionCount:{})", self.sections().count());
    return os;
  }
};

}  // namespace binspect
