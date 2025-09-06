#include "binspect/Binary.h"
#include "binspect/FD.h"
#include "binspect/MMap.h"

#include <cassert>
#include <iostream>
#include <string_view>

void test(std::string_view path) {
  auto fd = binspect::FD::open(path);
  auto mm = binspect::MMap::mapFile(std::move(fd));
  auto bin = binspect::Binary::at(mm->addr_);
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
