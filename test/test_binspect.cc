#include "binspect/context.h"
#include "binspect/fd.h"
#include "binspect/mmap.h"

#include <cassert>
#include <iostream>
#include <string_view>

size_t allocs_ {};
size_t deallocs_ {};
size_t alloc_total_ {};
size_t dealloc_total_ {};
size_t peak_ {};

void dump_stats() {
  if (allocs_ || deallocs_) {
    std::cerr
        << std::format(
               "allocs: {} ({} bytes) deallocs: {} ({} bytes); peak {} bytes",
               allocs_,
               alloc_total_,
               deallocs_,
               dealloc_total_,
               peak_)
        << '\n';
  }
  assert(allocs_ == deallocs_);
  assert(alloc_total_ == dealloc_total_);
}

namespace {
template <typename T>
struct test_alloc : std::allocator<T> {
  T* allocate(size_t count) {
    ++allocs_;
    alloc_total_ += count;
    peak_ = std::max(peak_, alloc_total_ - dealloc_total_);
    return ::new T[count];
  }

  void deallocate(T* ptr, size_t count) {
    ++deallocs_;
    dealloc_total_ += count;
    ::operator delete[](ptr, count);
  }

  template <typename U>
  struct rebind {
    using other = test_alloc<U>;
  };
};
}  // namespace

void test(std::string_view path) {
  test_alloc<std::byte> alloc;
  binspect::alloc_resource resource {alloc};
  binspect::heap heap {resource};
  binspect::context cx {heap};

  auto fd = binspect::fd::open(path);
  auto mm = binspect::mmap::map_file(std::move(fd));
  auto bin = cx.binary_at(mm->addr_);
  assert(bin);

  std::cout << *bin << '\n';
  for (auto section : bin->sections()) {
    std::cout << "... " << section << '\n';
    (void) section;
  }

  for (auto symbol : bin->symbols()) {
    std::cout << "... " << symbol << '\n';
    (void) symbol;
  }
}

int main(int argc, char** argv) {
  assert(argc == 2);  // expect binary file to test with as the only arg
  test(argv[1]);      // actually do the tests within `test`
  dump_stats();       // at this point everything's out of scope, so check alloc stats
  return 0;
}
