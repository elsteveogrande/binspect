#include "binspect/fd.h"
#include "binspect/mmap.h"

#include <cassert>

int main(int, char** argv) {
  auto f = *binspect::fd::open(argv[0]);
  auto m = *binspect::mmap::map_file(std::move(f));
  assert(m.addr_);
  auto* b = (uint8_t*) m.addr_;
  assert(*b);
  return 0;
}
