#pragma once

#include <cassert>
#include <functional>
#include <ranges>
#include <type_traits>

namespace binspect {

struct context;

// clang-format off
template <class T>
struct view : std::ranges::view_interface<view<T>> {
  std::function<T(size_t)> at_ {};
  std::function<size_t()> count_ {};

  static view empty() { return {.at_ = nullptr, .count_ = [] {return 0;}}; }

  template <class C,
            typename V = typename C::value_type>
            requires requires (C const& c) {
              std::is_convertible_v<V, T>
              && std::is_integral_v<decltype(c.size())>
              && std::is_same_v<V const&, decltype(c.at(size_t{}))>; }
  static view<T> of(C const& c) {
    return {
      .at_ = [&c] (size_t i) { return c.at[i]; },
      .count_ = [&c] { return c.size(); }
    };
  }

  template <class X>
  struct __rand_access {
    view const& view_;
    size_t i_ {};
    X operator*() const { return view_.at_(i_); }
    auto operator++(int) { auto ret = *this; ++i_; return *ret; }
    auto operator++() { ++i_; return *this; }
    bool operator==(auto const& rhs) const { return i_ == rhs.i_; }
  };

  __rand_access<T> begin() const {
    if (at_ && count_) { return __rand_access<T>{*this, 0}; }
    assert (false && "cannot support iteration");
  }

  __rand_access<T> end() const {
    if (at_ && count_) { return __rand_access<T>{*this, count_()}; }
    assert (false && "cannot support iteration");
  }

  size_t count() const { return count_(); }
  T const& at(size_t index) const { return at_(index); }
  T const& operator[](size_t index) const { return at_(index); }
};
static_assert(std::semiregular<view</* dummy T */ int>>);
// clang-format on

}  // namespace binspect
