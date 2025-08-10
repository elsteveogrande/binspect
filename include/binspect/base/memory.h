#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <memory_resource>

namespace binspect {

struct alloc_resource final : std::pmr::memory_resource {
  std::function<std::byte*(size_t)> __alloc_bytes;
  std::function<void(std::byte*, size_t)> __dealloc_bytes;

  template <class A>
  explicit alloc_resource(A const& alloc) {
    using AT = typename std::allocator_traits<A>;
    using BA = typename AT::template rebind_alloc<std::byte>;
    auto ba = BA(alloc);
    __alloc_bytes = [ba](size_t sz) mutable { return (std::byte*) ba.allocate(sz); };
    __dealloc_bytes = [ba](std::byte* ptr, size_t sz) mutable {
      return ba.deallocate(ptr, sz);
    };
  }

  virtual ~alloc_resource() = default;

  virtual bool do_is_equal(std::pmr::memory_resource const& rhs) const noexcept {
    return std::addressof(rhs) == this;
  }

  virtual void* do_allocate(size_t bytes, size_t align) {
    auto* ret = (std::byte*) __alloc_bytes(bytes);
    auto addr = uintptr_t(ret);
    assert(!(addr % align));  ////////////////////////////// FIXME
    return ret;
  }

  virtual void do_deallocate(void* ptr, size_t bytes, size_t) {
    __dealloc_bytes((std::byte*) ptr, bytes);
  }
};

struct heap {
  std::pmr::memory_resource* rs_;
  std::pmr::unsynchronized_pool_resource heap_;
  explicit heap(std::pmr::memory_resource& rs) : rs_(&rs), heap_(rs_) {}
};

struct bump {
  std::pmr::memory_resource* rs_;
  std::pmr::monotonic_buffer_resource bump_;
  explicit bump(std::pmr::memory_resource& rs) : rs_(&rs), bump_(rs_) {}
};

}  // namespace binspect
