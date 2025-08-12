#pragma once

#include "binspect/view.h"

#include <format>
#include <ostream>
#include <print>
#include <string_view>

namespace binspect {

struct context;

struct section final {
  uintptr_t vm_addr;
  std::string_view name;
  std::byte const* content;
  std::byte const* content_end;

  friend std::ostream& operator<<(std::ostream& os, section const& self) {
    std::print(
        os,
        "(section vm_addr:0x{:012x} size:{} name:{})",
        self.vm_addr,
        (self.content_end - self.content),
        self.name);
    return os;
  }
};

struct symbol final {
  uintptr_t value;
  std::string_view name;
  friend std::ostream& operator<<(std::ostream& os, symbol const& self) {
    std::print(os, "(symbol value:0x{:012x} name:{})", self.value, self.name);
    return os;
  }
};

struct binary final {
  context& cx_;
  std::function<view<section>()> sections {};
  std::function<view<symbol>()> symbols {};

  friend std::ostream& operator<<(std::ostream& os, binary const& self) {
    std::print(os, "(binary sectionCount:{})", self.sections().count());
    return os;
  }
};

}  // namespace binspect
