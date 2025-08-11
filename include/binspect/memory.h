#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <memory_resource>

namespace binspect {

/**
 * Our version of the experimental `resource_adaptor` (some stuff omitted for brevity):
 * https://en.cppreference.com/w/cpp/experimental/resource_adaptor.html
 *
 * tl;dr: accepts an allocator, recasts to byte allocator, and wraps in
 * pmr::memory_resource.
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

  void* do_allocate(size_t bytes, size_t align) override {
    auto* ret = (std::byte*) __alloc_bytes(bytes);
    auto addr = uintptr_t(ret);
    assert(!(addr % align));  ////////////////////////////// FIXME
    return ret;
  }

  void do_deallocate(void* ptr, size_t bytes, size_t) override {
    __dealloc_bytes((std::byte*) ptr, bytes);
  }
};

/**
 * The "heap" is a scope-private memory resource is what we generally allocate everything
 * onto (except temporaries).  It will be the storage for anything returned by the
 * binspect API, so it should outlive the binspect context it's given to.
 *
 * It should be in use for only one context at a time.  This class is not thread-safe.
 */
struct heap {
  std::pmr::memory_resource& rs_;
  std::pmr::unsynchronized_pool_resource heap_;

  /** Build a heap, using plain old new/delete as upstream memory resource. */
  heap() : heap(*std::pmr::new_delete_resource()) {}
  /** Build a heap using a provided upstream resource. */
  explicit heap(std::pmr::memory_resource& rs) : rs_(rs), heap_(&rs_) {}
};

}  // namespace binspect
