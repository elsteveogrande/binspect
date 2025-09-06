#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <bit>
#include <cstdint>
#include <type_traits>

namespace binspect {

template <typename U>
concept UWord = requires {
  std::is_same_v<U, uint16_t> || std::is_same_v<U, uint32_t> || std::is_same_v<U, uint64_t>;
};

template <UWord Z>
struct Swapped {
  Z const __val_;
  constexpr operator Z() const { return std::byteswap(__val_); }
};

template <bool V>
struct _NativeIsLE {};

template <>
struct _NativeIsLE<true> {
  using U16LE = uint16_t;
  using U32LE = uint32_t;
  using U64LE = uint64_t;
  using U16BE = Swapped<uint16_t>;
  using U32BE = Swapped<uint32_t>;
  using U64BE = Swapped<uint64_t>;
};

template <>
struct _NativeIsLE<false> {
  using U16BE = uint16_t;
  using U32BE = uint32_t;
  using U64BE = uint64_t;
  using U16LE = Swapped<uint16_t>;
  using U32LE = Swapped<uint32_t>;
  using U64LE = Swapped<uint64_t>;
};

using _Endians = _NativeIsLE<std::endian::native == std::endian::little>;
static_assert(sizeof(_Endians::U16LE) == 2);
static_assert(sizeof(_Endians::U16BE) == 2);
static_assert(sizeof(_Endians::U32LE) == 4);
static_assert(sizeof(_Endians::U32BE) == 4);
static_assert(sizeof(_Endians::U64LE) == 8);
static_assert(sizeof(_Endians::U64BE) == 8);

}  // namespace binspect
