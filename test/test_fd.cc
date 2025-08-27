#include "binspect/fd.h"

#include <cassert>

int main(int, char** argv) {
  auto f = binspect::fd::open(argv[0]);
  assert(f);
  assert(f->size());
  assert(*f->size() > 1000);
  return 0;
}
