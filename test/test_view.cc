#include "binspect/view.h"

#include <array>
#include <cassert>

int main(int, char**) {
  std::array<int, 5> ints {11, 22, 33, 44, 55};
  auto v = binspect::view<int>::of(ints);
  assert(v[0] == 11);
  assert(v[4] == 55);
  return 0;
}
