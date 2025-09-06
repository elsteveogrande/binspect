#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <format>
#include <string_view>

namespace binspect {

struct Symbol final {
  enum class Flag : uint32_t {
    GLOBAL = 1u << 31,
    LOCAL = 1u << 30,
    WEAK = 1u << 29,

    DATA = 1u << 2,
    CODE = 1u << 1,
    OTHER = 1u << 0,

    _UNKNOWN = 0
  };

  uintptr_t const value;
  std::string_view const name;
  uint32_t const flags;
  std::optional<size_t> const size {};

  /** Return a C string representing the flag name, or `(UNKNOWN)` */
  constexpr static char const* flagName(binspect::Symbol::Flag f) {
    switch (f) {
    case binspect::Symbol::Flag::LOCAL:  return "LOCAL";
    case binspect::Symbol::Flag::GLOBAL: return "GLOBAL";
    case binspect::Symbol::Flag::WEAK:   return "WEAK";
    case binspect::Symbol::Flag::DATA:   return "DATA";
    case binspect::Symbol::Flag::CODE:   return "CODE";
    case binspect::Symbol::Flag::OTHER:  return "OTHER";
    default:  // pass-through
    }
    return "(UNKNOWN)";
  }
};

}  // namespace binspect

template <>
struct std::formatter<binspect::Symbol::Flag> {
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto flag, auto& cx) const {
    return std::format_to(cx.out(), "{}", binspect::Symbol::flagName(flag));
  }
};

template <>
struct std::formatter<binspect::Symbol> {
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto const& sym, auto& cx) const {
    return std::format_to(
        cx.out(),
        "(symbol value:0x{:012x} flags:0x{:08x} size:{} name:{})",
        sym.value,
        sym.flags,
        sym.size.value_or(-1),
        sym.name);
  }
};
