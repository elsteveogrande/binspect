#include "binspect/binspect.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

int main(int argc, char** argv) {
  assert(argc == 2);
  std::string_view path {argv[1]};

  std::allocator<std::byte> alloc;
  binspect::alloc_resource alloc_rsrc {alloc};
  binspect::heap heap {alloc_rsrc};
  binspect::context cx {heap};

  auto fd = binspect::fd::open(path);
  assert(fd);
  auto mm = binspect::mmap::map_file(std::move(*fd));
  assert(mm);
  auto bin = cx.binary_at(mm->addr_);
  assert(bin);

  std::cout << bin.value() << '\n';
  for (auto section : bin.value().sections()) { std::cout << "... " << section << '\n'; }

  return 0;
}
