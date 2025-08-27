#pragma once

#include "binspect/error.h"

#include <cassert>
#include <cstddef>
#include <expected>
#if !defined(__cpp_exceptions)
#include <iostream>
#include <print>
#endif

namespace binspect {

template <class T>
struct res : std::expected<T, error> {
  using X = std::expected<T, error>;

  operator bool() const { return this->has_value(); }

  static void __throw(error const& err) {
#if defined(__cpp_exceptions)
    throw err;
#else
    std::println(std::cerr, "{}", err);
    std::unreachable();
#endif
  }

  res() {}
  res(error err) : X(std::unexpected {std::move(err)}) {}

  template <class... A>
  res(A&&... args) : X {std::in_place_t {}, std::forward<A>(args)...} {}
};

}  // namespace binspect
