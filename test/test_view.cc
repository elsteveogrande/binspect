#include "binspect/view.h"

#include <array>
#include <cassert>
#include <print>

int main(int, char**) {
  std::array<int, 5> ints {11, 22, 33, 44, 55};
  auto c = binspect::view<int>::of(ints);
  auto at_ = [&ints](size_t i) { return ints.at(i); };
  std::println("{} {} {}", ints.at(0), at_(0), c.at(0));
  assert(c[0] == 11);
  assert(c[4] == 55);
  return 0;
}
