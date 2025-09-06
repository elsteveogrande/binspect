#include "binspect/FD.h"
#include "binspect/MMap.h"

#include <cassert>

int main(int, char** argv) {
  binspect::FD f(argv[0]);
  assert(f.ok());
  binspect::MMap m(std::move(f));
  assert(m.ok());
  assert(m.data_);
  auto* b = (uint8_t*) m.data_;
  assert(b);
  assert(*b);
  return 0;
}
