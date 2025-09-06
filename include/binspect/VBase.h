#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace binspect {

template <
    class V,                              // Base class type w/ virtual functions
    size_t S = 2 * sizeof(long),          // Default is 2 words (vtable + one pointer)
    size_t A = alignof(std::max_align_t)  // Expects default alignment
    >
struct VBase {
  /** V is presumed to have virtual methods, however should have no destructor (not even a
   * virtual default dtor). */
  static_assert(std::is_trivially_destructible_v<V>);
  static_assert(std::is_polymorphic_v<V>);

  /** Bytes representing the object; copied (by `memcpy` into this) */
  [[gnu::aligned(A)]] char data_[S];

  /** Contains an object and not just zeroes. */
  constexpr bool ok() const {
    return *reinterpret_cast<uintptr_t const*>(data_);  // Any V should have non-null vtable ptr
  }

  constexpr V const* ptr() const { return reinterpret_cast<V const*>(data_); }
  constexpr V const* operator->() const { return ptr(); }
  constexpr V const& operator*() const { return *ptr(); }

  constexpr void clear() { memset(data_, 0, S); }

  constexpr ~VBase() { clear(); }
  constexpr VBase() { clear(); }
  constexpr VBase(VBase const&) = default;
  constexpr VBase& operator=(VBase const&) = default;

  constexpr VBase(auto&& obj) { assign(obj); }
  constexpr VBase& operator=(auto&& obj) { return assign(obj); }

  constexpr VBase(auto const& obj) { assign(obj); }
  constexpr VBase& operator=(auto const& obj) { return assign(obj); }

  constexpr VBase& assign(auto const& obj) {
    using T = std::remove_const_t<std::remove_reference_t<decltype(obj)>>;
    static_assert(sizeof(T) <= S);
    static_assert(alignof(T) <= A);
    static_assert(std::is_trivially_destructible_v<T>);  // Can throw away data w/ no cleanup
    static_assert(std::is_final_v<T>);                   // Avoids "clipping" data beyond size S
    static_assert(std::is_base_of_v<V, T>);              // T must inherits from V
    std::memcpy(data_, (void*) &obj, S);
    return *this;
  }
};

}  // namespace binspect
