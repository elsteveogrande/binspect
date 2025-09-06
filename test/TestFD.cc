#include "binspect/FD.h"

#include <cassert>

int main(int, char** argv) {
  binspect::FD f(argv[0]);
  assert(f.ok());
  assert(f.stat().st_size > 1000);

  return 0;
}
