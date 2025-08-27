#pragma once

#include <cassert>
#include <cstring>
#include <format>
#include <string_view>

namespace binspect {

struct error {
  std::optional<int> errno_ {-1};  // used w/ C calls which returns err code / uses errno
  std::string_view msg_ {};        // optional, default empty string

  std::string_view class_name() const {
#if defined(__cpp_rtti)
    return typeid(*this).name();
#else
    return "binspect::error";
#endif
  }
};

}  // namespace binspect

template <>
struct std::formatter<binspect::error, char> {
  constexpr auto parse(auto& cx) { return cx.begin(); }
  auto format(binspect::error const& e, auto& cx) const {
    auto cn = e.class_name();  // XXX demangle this
    auto ec = e.errno_.value_or(0);
    auto* se = ec ? strerror(ec) : nullptr;
    auto em = e.msg_;
    cx.advance_to(std::format_to(cx.out(), "{}", cn));
    if (ec) { cx.advance_to(std::format_to(cx.out(), ": (errno {}, {})", ec, se)); }
    if (!em.empty()) { cx.advance_to(std::format_to(cx.out(), ": {}", em)); }
    return cx.out();
  }
};
