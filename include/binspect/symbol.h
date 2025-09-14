#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <format>
#include <string_view>

namespace binspect {

struct Symbol final {
  enum class Binding : uint8_t { OTHER = 0, LOCAL, GLOBAL, WEAK };
  enum class Type : uint8_t { OTHER = 0, CODE, DATA, SECTION, FILE };
  enum class Visibility : uint8_t { OTHER = 0, DEFAULT, PROTECTED, HIDDEN };

  uintptr_t value;
  std::string_view name;
  Binding binding {};
  Type type {};
  Visibility visibility {};
  size_t size {};  // 0 iff unknown/undefined
};

}  // namespace binspect

template <>
struct std::formatter<binspect::Symbol::Binding> {
  using E = binspect::Symbol::Binding;
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto flag, auto& cx) const {
    char const* str {};
    switch (flag) {
    case E::LOCAL:  str = "LOCAL"; break;
    case E::GLOBAL: str = "GLOBAL"; break;
    case E::WEAK:   str = "WEAK"; break;
    case E::OTHER:  str = "OTHER"; break;
    default:        str = "(?)";
    }
    return std::format_to(cx.out(), "{}", str);
  }
};

template <>
struct std::formatter<binspect::Symbol::Type> {
  using E = binspect::Symbol::Type;
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto flag, auto& cx) const {
    char const* str {};
    switch (flag) {
    case E::CODE:    str = "CODE"; break;
    case E::DATA:    str = "DATA"; break;
    case E::SECTION: str = "SECTION"; break;
    case E::FILE:    str = "FILE"; break;
    case E::OTHER:   str = "OTHER"; break;
    default:         str = "(?)";
    }
    return std::format_to(cx.out(), "{}", str);
  }
};

template <>
struct std::formatter<binspect::Symbol::Visibility> {
  using E = binspect::Symbol::Visibility;
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto flag, auto& cx) const {
    char const* str {};
    switch (flag) {
    case E::DEFAULT:   str = "DEFAULT"; break;
    case E::HIDDEN:    str = "HIDDEN"; break;
    case E::PROTECTED: str = "PROTECTED"; break;
    case E::OTHER:     str = "OTHER"; break;
    default:           str = "(?)";
    }
    return std::format_to(cx.out(), "{}", str);
  }
};

template <>
struct std::formatter<binspect::Symbol> {
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto const& sym, auto& cx) const {
    return std::format_to(
        cx.out(),
        "(symbol value:0x{:012x} binding:{} type:{} visibility:{} size:{} name:{})",
        sym.value,
        sym.binding,
        sym.type,
        sym.visibility,
        sym.size,
        sym.name);
  }
};
