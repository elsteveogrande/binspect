#include "binspect/Binary.h"
#include "binspect/FD.h"
#include "binspect/MMap.h"

#include <cassert>
#include <print>
#include <string_view>

void test(std::string_view path) {
  binspect::FD fd(path);
  binspect::MMap mm(std::move(fd));
  binspect::Binary bin(std::move(mm));
  assert(bin.ok());
  for (auto sec : bin.sections()) { std::print("{}\n", sec); }
  // for (auto sym : bin->symbols()) { std::print("{}\n", sym); }
}

int main(int argc, char** argv) {
  assert(argc == 2);  // expect binary file to test with as the only arg
  test(argv[1]);      // actually do the tests within `test`
  return 0;
}
