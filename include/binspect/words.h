#pragma once

#include <bit>
#include <cstdint>
#include <type_traits>

namespace binspect {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
static_assert(sizeof(u16) == 2);

template <typename U>
concept uword = requires {
  std::is_same_v<U, u16> || std::is_same_v<U, u32> || std::is_same_v<U, u64>;
};

template <uword Z>
struct swapped {
  Z const __val_;
  constexpr operator Z() const { return std::byteswap(__val_); }
};

template <bool V>
struct __native_is_le {};

template <>
struct __native_is_le<true> {
  using u16le = u16;
  using u32le = u32;
  using u64le = u64;
  using u16be = swapped<u16>;
  using u32be = swapped<u32>;
  using u64be = swapped<u64>;
};

template <>
struct __native_is_le<false> {
  using u16be = u16;
  using u32be = u32;
  using u64be = u64;
  using u16le = swapped<u16>;
  using u32le = swapped<u32>;
  using u64le = swapped<u64>;
};

using __endians = __native_is_le<std::endian::native == std::endian::little>;
static_assert(sizeof(__endians::u16le) == 2);
static_assert(sizeof(__endians::u16be) == 2);
static_assert(sizeof(__endians::u32le) == 4);
static_assert(sizeof(__endians::u32be) == 4);
static_assert(sizeof(__endians::u64le) == 8);
static_assert(sizeof(__endians::u64be) == 8);

// // //

namespace __test_words {
constexpr u8 __bytes[4] = {0x78, 0x56, 0x34, 0x12};
constexpr __endians::u32le __test32le = std::bit_cast<__endians::u32le>(__bytes);
static_assert(__test32le == 0x12345678);
constexpr __endians::u32be __test32be = std::bit_cast<__endians::u32be>(__bytes);
static_assert(__test32be == 0x78563412);
}  // namespace __test_words

}  // namespace binspect
