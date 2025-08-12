#pragma once

#include "binspect/view.h"

#include <format>
#include <optional>
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

  size_t size() const { return size_t(content_end - content); }

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
  enum class flag : uint32_t {
    global = 1u << 31,
    local = 1u << 30,
    weak = 1u << 29,

    data = 1u << 2,
    code = 1u << 1,
    other = 1u << 0,

    __unknown = 0
  };

  uintptr_t value;
  std::string_view name;
  uint32_t flags;
  std::optional<size_t> size {};

  friend std::ostream& operator<<(std::ostream& os, symbol const& self) {
    std::print(
        os,
        "(symbol value:0x{:012x} flags:0x{:08x} size:{} name:{})",
        self.value,
        self.flags,
        self.size.value_or(-1),
        self.name);
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

template <>
struct std::formatter<binspect::symbol::flag> {
  auto format(binspect::symbol::flag f, format_context&) const {
    switch (f) {
    case binspect::symbol::flag::local:
    case binspect::symbol::flag::global:
    case binspect::symbol::flag::weak:
    case binspect::symbol::flag::data:
    case binspect::symbol::flag::code:
    case binspect::symbol::flag::other:
    default:                             return "?";
    }
  }
};
