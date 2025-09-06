#include "binspect/FD.h"
#include "binspect/MMap.h"

#include <cassert>

int main(int, char** argv) {
  auto f = binspect::FD::open(argv[0]);
  assert(f);
  auto m = binspect::MMap::mapFile(std::move(*f));
  assert(m);
  assert(m->addr_);
  auto* b = (uint8_t*) m->addr_;
  assert(b);
  assert(*b);
  return 0;
}
