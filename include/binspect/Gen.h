#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <cassert>
#include <coroutine>
#include <exception>
#include <iterator>

namespace binspect {
// clang-format off

template <class T> struct Promise;
template <class T> struct Gen;
template <class T> using Handle = std::coroutine_handle<Promise<T>>;

template <class T> struct Promise {
  Gen<T>* gen_ {};

  Gen<T> get_return_object() noexcept { return {Handle<T>::from_promise(*this)}; }

  std::suspend_never initial_suspend() const noexcept { return {}; }
  std::suspend_never final_suspend() noexcept { gen_->done_ = true; return {}; }
  void unhandled_exception() { gen_->exc_ = std::current_exception(); gen_->done_ = true; }
  std::suspend_always yield_value(T val) { gen_->val_ = std::move(val); return {}; }

  // FIXME this needs overridden new/delete to avoid heap allocation!

  // constexpr static size_t kSize = 1024;
  // static void* bytes() {
  //   thread_local static char bytes_[kSize * 2] {};
  //   thread_local static auto ret = (uintptr_t(bytes_) + kSize - 1) & !(kSize - 1);
  //   return reinterpret_cast<void*>(ret);
  // }
  // static bool& busy() {
  //   thread_local static bool busy_ {};
  //   return busy_;
  // }

  // void* operator new(size_t sz) noexcept {
  //   assert (!busy());
  //   busy() = true;
  //   assert (sz <= kSize);
  //   return bytes();
  // }

  // void operator delete(void*) {
  //   assert (busy());
  //   busy() = false;
  //  }
};

template <class T>
struct Iterator {
  Gen<T>& gen_;
  bool operator==(std::default_sentinel_t) const { return gen_.done_; }
  T& operator*() { return gen_.val_; }
  Iterator operator++(int) { return ++*this; }
  Iterator operator++() { gen_.co_.resume(); return *this; }
};

template <class T>
struct Gen : std::ranges::view_interface<Gen<T>> {
  using promise_type = Promise<T>;

  Handle<T> co_ {};
  T val_ {};
  std::exception_ptr exc_ {};
  bool done_ {};

  Gen(Gen const&) = delete;
  Gen& operator=(Gen const&) = delete;
  Gen(Gen&&) = default;
  Gen& operator=(Gen&&) = default;

  auto begin() { return Iterator {*this}; }
  auto end() { return std::default_sentinel; }

  Gen(std::coroutine_handle<Promise<T>> co) : co_(std::move(co)) { co_.promise().gen_ = this; }
};

}  // namespace binspect
