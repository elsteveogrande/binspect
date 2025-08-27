#include "binspect/error.h"

#include <cassert>
#include <print>

int main(int, char**) {
  binspect::error e {.errno_ = ENOMEM};
  auto str = std::format("{}", e);
  std::print("{}\n", str);
  // Not sure how portable ENOMEM being 12 is, but works on macos and linux
  assert(str == "N8binspect5errorE: (errno 12, Cannot allocate memory)");
  return 0;
}
