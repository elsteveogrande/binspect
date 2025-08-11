#pragma once

#include "binspect/error.h"

#include <cassert>
#include <cstddef>
#include <expected>
#include <functional>
#include <memory>
#include <memory_resource>

namespace binspect {

/**
 * Our version of the experimental `resource_adaptor` (some stuff omitted for brevity):
 * https://en.cppreference.com/w/cpp/experimental/resource_adaptor.html
 *
 * tl;dr: accepts an allocator, recasts as byte allocator, wraps in pmr::memory_resource.
 * Mostly this is just redirecting `do_allocate` to allocator's `allocate`, and so on,
 * with a bit of extra work to make sure alignment (which is supported by memory_resource
 * but not by allocator) is observed.
 */
struct alloc_resource final : std::pmr::memory_resource {
  std::function<std::byte*(size_t)> __alloc_bytes;
  std::function<void(std::byte*, size_t)> __dealloc_bytes;

  template <
      class A,
      class AT = typename std::allocator_traits<A>,
      class BA = typename AT::template rebind_alloc<std::byte>>
  explicit alloc_resource(A const& alloc) {
    auto ba = BA(alloc);
    __alloc_bytes = [ba](size_t sz) mutable { return (std::byte*) ba.allocate(sz); };
    __dealloc_bytes = [ba](std::byte* ptr, size_t sz) mutable {
      return ba.deallocate(ptr, sz);
    };
  }

  virtual ~alloc_resource() = default;

  bool do_is_equal(std::pmr::memory_resource const& rhs) const noexcept override {
    return std::addressof(rhs) == this;
  }

  struct token final {
    uint32_t padding_;
    uint32_t requested_;

    static token* after(void* obj, size_t bytes) {
      auto space = sizeof(padding_);
      auto* ret = (std::byte*) obj + bytes;
      std::align(alignof(uint32_t), bytes, (void*&) ret, space);
      return (token*) ret;
    }
  };
  static_assert(sizeof(token) == 8);
  static_assert(alignof(token) == 4);

  void* do_allocate(size_t bytes, size_t align) override {
    assert(bytes < (1 << 20));                            // reject nonsense
    assert(align < (1 << 16));                            // reject nonsense
    auto request = bytes + align + sizeof(uint32_t) - 1;  // bytes needed, worst-case
    auto* ret = __alloc_bytes(request);                   // acquire bytes, incl. overhead
    auto* orig = ret;                                     // remember orig pointer
    auto total = request;                                 // total size of allocation
    std::align(align, bytes, (void*&) ret, total);        // align ret with padding
    auto* tok = token::after(ret, bytes);                 // word goes just after obj
    tok->padding_ = uint32_t(ret - orig);                 // indicate amount of padding
    tok->requested_ = uint32_t(request);                  // needed for dealloc call
    return (void*) ret;
  }

  void do_deallocate(void* obj, size_t bytes, size_t /* align ignored */) override {
    auto* tok = token::after(obj, bytes);                // word is just after obj
    auto* ptr = (std::byte*) obj - tok->padding_;        // back up past padding bytes
    __dealloc_bytes((std::byte*) ptr, tok->requested_);  // orig (ptr, size) from allocate
  }
};

/**
 * The "heap" is a scoped memory resource and is what we generally allocate everything
 * onto, except data we know are temporary (which we'll use `bump` for).  It will be the
 * storage for anything returned by the binspect API, so it should outlive the binspect
 * context it's given to.
 *
 * It should be in use for only one context at a time.  This class is not thread-safe.
 */
struct heap : std::pmr::unsynchronized_pool_resource {
  using base = std::pmr::unsynchronized_pool_resource;

  virtual ~heap() = default;

  /** Build a heap using a provided upstream resource. */
  explicit heap(std::pmr::memory_resource& rs) : base(&rs) {}

  template <typename T>
  std::pmr::polymorphic_allocator<T> alloc() {
    return {this};
  }
};

template <class T>
struct ref : private std::expected<std::shared_ptr<T>, error> {
  using SP = std::shared_ptr<T>;
  using X = std::expected<SP, error>;

  /** Truthy IFF this is not empty, and not error. */
  operator bool() const { return this->has_value() && this->value().get(); }

  static void __throw(error const& err) {
#if defined(__cpp_exceptions) && __cpp_exceptions >= 199711L
    throw err;
#else
    std::cerr << err << '\n';
    abort();
#endif
  }

  /** Return pointer to value if present, else throw error */
  T* operator->(this auto& self) { return &*self; }

  /** Return reference to value if present, else throw error */
  T& operator*(this auto& self) {
    if (!self.has_value()) { __throw(self.error()); }
    if (!self.value().get()) { __throw(error(-1)); }
    return *self.value().get();
  }

  ref() {}
  ref(SP sp) : X(std::move(sp)) {}
  ref(error err) : X(std::unexpected {std::move(err)}) {}

  template <class... A>
  explicit ref(heap& heap, A&&... args)
      : ref {std::allocate_shared<T>(heap.alloc<T>(), std::forward<A>(args)...)} {}
};

}  // namespace binspect
