#include "binspect/context.h"
#include "binspect/fd.h"
#include "binspect/mmap.h"

#include <cassert>
#include <iostream>
#include <string_view>

void test(std::string_view path) {
  binspect::context cx;
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
  return 0;
}
