#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <format>
#include <string_view>

namespace binspect {

struct Section final {
  uintptr_t const addr;
  std::string_view const name;
  std::byte const* const content;
  std::byte const* const contentEnd;

  size_t size() const { return size_t(contentEnd - content); }
};

}  // namespace binspect

template <>
struct std::formatter<binspect::Section> {
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(auto const& sec, auto& cx) const {
    return std::format_to(
        cx.out(), "(section addr:0x{:012x} size:{} name:{})", sec.addr, sec.size(), sec.name);
  }
};
