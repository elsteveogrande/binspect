#include "binspect/context.h"
#include "binspect/fd.h"
#include "binspect/memory.h"
#include "binspect/mmap.h"

#include <cassert>
#include <iostream>
#include <string_view>
#include <utility>

int main(int argc, char** argv) {
  assert(argc == 2);
  std::string_view path {argv[1]};

  binspect::heap heap;
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
