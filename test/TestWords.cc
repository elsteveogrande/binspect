#include "binspect/Words.h"

#include <cassert>

int main() {
  constexpr uint8_t __bytes[4] = {0x78, 0x56, 0x34, 0x12};
  constexpr auto __test32le = std::bit_cast<binspect::LE::U32>(__bytes);
  assert(__test32le == 0x12345678);
  constexpr auto __test32be = std::bit_cast<binspect::BE::U32>(__bytes);
  assert(__test32be == 0x78563412);
  return 0;
}
