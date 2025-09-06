#include "binspect/Words.h"

#include <cassert>

int main() {
  using Endians = binspect::_Endians;
  constexpr uint8_t __bytes[4] = {0x78, 0x56, 0x34, 0x12};
  constexpr Endians::U32LE __test32le = std::bit_cast<Endians::U32LE>(__bytes);
  assert(__test32le == 0x12345678);
  constexpr Endians::U32BE __test32be = std::bit_cast<Endians::U32BE>(__bytes);
  assert(__test32be == 0x78563412);
  return 0;
}
