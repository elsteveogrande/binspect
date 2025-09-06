#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <format>
#include <optional>
#include <print>
#include <string_view>
#include <utility>

namespace binspect {

struct Symbol final {
  enum class Flag : uint32_t {
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

  // friend std::ostream& operator<<(std::ostream& os, Symbol const& self) {
  //   std::print(
  //       os,
  //       "(symbol value:0x{:012x} flags:0x{:08x} size:{} name:{})",
  //       self.value,
  //       self.flags,
  //       self.size.value_or(-1),
  //       self.name);
  //   return os;
  // }
};

struct Symbols final : std::ranges::view_interface<Symbol> {};

}  // namespace binspect

template <>
struct std::formatter<binspect::Symbol::Flag> {
  auto format(binspect::Symbol::Flag f, std::format_context&) const {
    switch (f) {
    case binspect::Symbol::Flag::local:  return "local";
    case binspect::Symbol::Flag::global: return "global";
    case binspect::Symbol::Flag::weak:   return "weak";
    case binspect::Symbol::Flag::data:   return "data";
    case binspect::Symbol::Flag::code:   return "code";
    case binspect::Symbol::Flag::other:  return "other";
    default:                             std::unreachable();
    }
  }
};
