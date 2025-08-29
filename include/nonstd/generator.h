// -*- C++ -*-
#ifndef __GENERATOR
#define __GENERATOR
#include <exception>
#if !(defined(__cpp_lib_generator))
#include <coroutine>
#include <memory>
#include <memory_resource>
#include <type_traits>
namespace std {

namespace __generator {

// https://en.cppreference.com/w/cpp/coroutine/generator.html

template <class AT, class R, class V, class Y>
struct base {
  struct promise_type {
    using VT = add_pointer_t<Y>;
    VT value_;
    exception_ptr except_;
  };
};

template <
    class R_,
    class V_,
    class A_,
    class R = conditional_t<std::is_void_v<V_>, R_&&, R_>,
    class V = conditional_t<std::is_void_v<V_>, remove_cvref_t<R>, V_>,
    class A = conditional_t<
        std::is_void_v<A_>,
        std::allocator<int>,
        typename allocator_traits<A_>::template rebind_alloc<int>>,
    class AT = allocator_traits<A>,
    class Y = std::conditional_t<std::is_reference_v<R>, R, R const&>,
    class B = base<AT, R, V, Y>>
  requires(
      std::is_pointer_v<typename AT::pointer>  // allocator_traits<A>::pointer is ptr type
      && !std::is_const_v<V>                   // value is a cv-unqualified object type.
      && !std::is_volatile_v<V> &&             // ...
      (std::is_reference_v<R> ||               // reference is either a reference type,
       (!std::is_const_v<R>                    // or a cv-unqualified object type
        && !std::is_volatile_v<R>              // ...
        && std::is_copy_constructible_v<R>     // that models copy_constructible.
        ))) struct base0 : B {
  using yielded = Y;
  using promise_type = B::promise_type;
};

}  // namespace __generator

template <class Ref, class V = void, class Allocator = void>
class generator
    : private __generator::base0<Ref, V, Allocator>,
      public ranges::view_interface<generator<Ref, V, Allocator>> {};

namespace pmr {
template <class Ref, class V = void>
using generator = std::generator<Ref, V, std::pmr::polymorphic_allocator<>>;
}  // namespace pmr

}  // namespace std
#endif  // !(defined (__cpp_lib_generator))
#endif  // __GENERATOR
