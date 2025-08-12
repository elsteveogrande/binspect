#pragma once

#include <bit>
#include <cstdint>
#include <type_traits>

namespace binspect {

template <typename U>
concept uword = requires {
  std::is_same_v<U, uint16_t> || std::is_same_v<U, uint32_t> || std::is_same_v<U, uint64_t>;
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
  using uint16_le = uint16_t;
  using uint32_le = uint32_t;
  using uint64_le = uint64_t;
  using uint16_be = swapped<uint16_t>;
  using uint32_be = swapped<uint32_t>;
  using uint64_be = swapped<uint64_t>;
};

template <>
struct __native_is_le<false> {
  using u16be = uint16_t;
  using uint32_be = uint32_t;
  using uint64_be = uint64_t;
  using u16le = swapped<uint16_t>;
  using u32le = swapped<uint32_t>;
  using u64le = swapped<uint64_t>;
};

using __endians = __native_is_le<std::endian::native == std::endian::little>;
static_assert(sizeof(__endians::uint16_le) == 2);
static_assert(sizeof(__endians::uint16_be) == 2);
static_assert(sizeof(__endians::uint32_le) == 4);
static_assert(sizeof(__endians::uint32_be) == 4);
static_assert(sizeof(__endians::uint64_le) == 8);
static_assert(sizeof(__endians::uint64_be) == 8);

// // //

namespace __test_words {
constexpr uint8_t __bytes[4] = {0x78, 0x56, 0x34, 0x12};
constexpr __endians::uint32_le __test32le = std::bit_cast<__endians::uint32_le>(__bytes);
static_assert(__test32le == 0x12345678);
constexpr __endians::uint32_be __test32be = std::bit_cast<__endians::uint32_be>(__bytes);
static_assert(__test32be == 0x78563412);
}  // namespace __test_words

}  // namespace binspect
