#pragma once

#include <binspect/base/error.h>
#include <binspect/base/memory.h>
#include <binspect/elf.h>
#include <expected>

namespace binspect {

struct context;

struct context {
  heap& heap_;
  bump bump_;
  // cache_sp cache_sp_{}; // optional

  context(context const&) = delete;
  context(context&&) = delete;
  context& operator=(context const&) = delete;
  context& operator=(context&&) = delete;

  ~context() {
    assert(thread_cx() == this);
    thread_cx() = nullptr;
  }

  explicit context(heap& heap) : heap_(heap), bump_(heap_.heap_) {
    assert(!thread_cx());
    thread_cx() = this;
  }

  static context& get() {
    auto* ret = thread_cx();
    assert(ret);
    return *ret;
  }

  std::expected<binary, error> binary_at(void const* ptr);

private:
  static context*& thread_cx() {
    static thread_local context* ret {nullptr};
    return ret;
  }
};

inline std::expected<binary, error> context::binary_at(void const* ptr) {
  if (ptr) {
    auto* e64le = (elf::elf64le const*) ptr;
    return binary {*this, [e64le]() { return e64le->sections_view(); }};
  }
  return {std::unexpected {error {}}};
}

}  // namespace binspect
