#include "binspect/context.h"
#include "binspect/fd.h"
#include "binspect/memory.h"
#include "binspect/mmap.h"

#include <cassert>
#include <iostream>
#include <string_view>
#include <utility>

namespace {
template <typename T> struct test_alloc : std::allocator<T> {
  T* allocate(size_t count) { return ::new T[count]; }
  void deallocate(T* ptr, size_t count) { ::operator delete[](ptr, count); }

  template <typename U> struct rebind {
    using other = test_alloc<U>;
  };
};
}  // namespace

int main(int argc, char** argv) {
  assert(argc == 2);
  std::string_view path {argv[1]};

  test_alloc<std::byte> alloc;
  binspect::alloc_resource resource {alloc};
  binspect::heap heap {resource};
  binspect::context cx {heap};

  auto fd = binspect::fd::open(path);
  assert(fd);
  auto mm = binspect::mmap::map_file(std::move(*fd));
  assert(mm);
  auto bin = cx.binary_at(mm->addr_);
  assert(bin);

  std::cout << *bin << '\n';
  for (auto section : bin->sections()) { std::cout << "... " << section << '\n'; }

  return 0;
}
