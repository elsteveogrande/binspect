#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/MMap.h"
#include "binspect/VBase.h"
#include "binspect/detail/ELF.h"

namespace binspect {

struct Binary final : VBase<BinaryBase, /* vtable + 1 word */ 3 * sizeof(long)> {
  using VB = VBase<BinaryBase, 3 * sizeof(long)>;
  using VB::VB;
  using VB::operator=;

  binspect::MMap owned_ {};
  binspect::MMap const& mm_;

  explicit Binary(binspect::MMap const& mm) : mm_(mm) { identify(); }
  explicit Binary(binspect::MMap&& mm) : owned_(std::move(mm)), mm_(owned_) { identify(); }

  void identify() {
    if (mm_.ok()) {
      if (elf::ELF64LE::valid(mm_.data_)) {
        assign(elf::ELF64LE(mm_.data_));
      } else if (elf::ELF32LE::valid(mm_.data_)) {
        assign(elf::ELF32LE(mm_.data_));
      } else if (elf::ELF64BE::valid(mm_.data_)) {
        assign(elf::ELF64BE(mm_.data_));
      } else if (elf::ELF32BE::valid(mm_.data_)) {
        assign(elf::ELF32BE(mm_.data_));
      }
    }
  }

  bool ok() const { return mm_.ok() && VB::ok(); }
};

}  // namespace binspect
