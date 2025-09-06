#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <bit>
#include <concepts>
#include <cstdint>

namespace binspect {

template <std::integral Z>
struct Swapped {
  Z const __val_;
  constexpr operator Z() const { return std::byteswap(__val_); }
};

struct NativeWords {
  using U16 = uint16_t;
  using U32 = uint32_t;
  using U64 = uint64_t;
};

struct SwappedWords {
  using U16 = Swapped<NativeWords::U16>;
  using U32 = Swapped<NativeWords::U32>;
  using U64 = Swapped<NativeWords::U64>;
};

template <bool V>
struct _IfNativeLE {};

template <>
struct _IfNativeLE<true> {
  using LE = NativeWords;
  using BE = SwappedWords;
};

template <>
struct _IfNativeLE<false> {
  using BE = NativeWords;
  using LE = SwappedWords;
};

using NativeE = _IfNativeLE<std::endian::native == std::endian::little>;
using LE = NativeE::LE;
using BE = NativeE::BE;

namespace detail {

template <class T, class X, class Y = int, class... Z>
consteval inline bool OneOf() {
  if (std::is_same_v<T, X>) { return true; }
  return OneOf<T, Y, Z...>();
}

template <class T, int>
consteval inline bool OneOf() {
  return false;
}

static_assert(OneOf<LE, LE, BE>());

}  // namespace detail

template <class E>
concept Endian = requires { detail::OneOf<E, LE, BE>; };

}  // namespace binspect
