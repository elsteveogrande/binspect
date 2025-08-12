#pragma once

#include "binspect/elf/elf32.h"
#include "binspect/elf/elf64.h"
#include "binspect/error.h"
#include "binspect/memory.h"

namespace binspect {

struct context {
  heap& heap_;

  context(context const&) = delete;
  context(context&&) = delete;
  context& operator=(context const&) = delete;
  context& operator=(context&&) = delete;

  ~context() {
    assert(thread_cx() == this);
    thread_cx() = nullptr;
  }

  explicit context(heap& heap) : heap_(heap) {
    assert(!thread_cx());
    thread_cx() = this;
  }

  static context& get() {
    auto* ret = thread_cx();
    assert(ret);
    return *ret;
  }

  ref<binary> binary_at(void const* ptr);

private:
  static context*& thread_cx() {
    static thread_local context* ret {nullptr};
    return ret;
  }
};

inline ref<binary> context::binary_at(void const* ptr) {
  if (ptr) {
    auto* e64le = (elf::elf64le const*) ptr;
    if (e64le->valid()) {
      return ref<binary>(
          heap_,
          *this,
          [e64le]() { return e64le->sections_view(); },
          [e64le]() { return e64le->symbols_view(); });
    }
    auto* e32le = (elf::elf32le const*) ptr;
    if (e32le->valid()) {
      return ref<binary>(
          heap_,
          *this,
          [e32le]() { return e32le->sections_view(); },
          [e32le]() { return e32le->symbols_view(); });
    }
    auto* e64be = (elf::elf64be const*) ptr;
    if (e64be->valid()) {
      return ref<binary>(
          heap_,
          *this,
          [e64be]() { return e64be->sections_view(); },
          [e64be]() { return e64be->symbols_view(); });
    }
    auto* e32be = (elf::elf32be const*) ptr;
    if (e32be->valid()) {
      return ref<binary>(
          heap_,
          *this,
          [e32be]() { return e32be->sections_view(); },
          [e32be]() { return e32be->symbols_view(); });
    }
  }
  return {error(-1)};
}

}  // namespace binspect
