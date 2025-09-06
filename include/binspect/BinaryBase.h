#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/Section.h"
// #include "binspect/Symbol.h"

#include <coroutine>

namespace binspect {

template <class X>
struct Gen {
  struct promise_type {
    std::suspend_always initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    std::suspend_always yield_value(X val) { return {}; }
    void unhandled_exception() { /* XXX */ }
    Gen<X> get_return_object() noexcept;
  };

  promise_type& pr_;
};
template <class X>
inline Gen<X> Gen<X>::promise_type::get_return_object() noexcept {
  return Gen<X> {*this};
}

struct BinaryBase {
  virtual Gen<Section> sections() const = 0;
  virtual Section section(uint16_t i) const = 0;

  // virtual View<Symbol> symbols() const = 0;
};

}  // namespace binspect
