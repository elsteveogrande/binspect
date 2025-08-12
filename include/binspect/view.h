#pragma once

#include <cassert>
#include <functional>
#include <ranges>

namespace binspect {

struct context;

// clang-format off
template <class T>
struct view : std::ranges::view_interface<view<T>> {
  std::function<T(size_t)> at {};
  std::function<size_t()> count {};

  static view empty() { return {.at=nullptr, .count=[] {return 0;}}; }

  template <class X>
  struct __rand_access {
    view const& view_;
    size_t i_ {};
    X operator*() const { return view_.at(i_); }
    auto operator++(int) { auto ret = *this; ++i_; return *ret; }
    auto operator++() { ++i_; return *this; }
    bool operator==(auto const& rhs) const { return i_ == rhs.i_; }
  };

  __rand_access<T> begin() const {
    if (at && count) { return __rand_access<T>{*this, 0}; }
    assert (false && "cannot support iteration");
  }

  __rand_access<T> end() const {
    if (at && count) { return __rand_access<T>{*this, count()}; }
    assert (false && "cannot support iteration");
  }
};
static_assert(std::semiregular<view</* dummy T */ int>>);
// clang-format on

}  // namespace binspect
