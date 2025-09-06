#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/ELF32.h"
#include "binspect/ELF64.h"
#include "binspect/MMap.h"
#include "binspect/VBase.h"

namespace binspect {

struct Binary final : VBase<BinaryBase, /* vtable + 2 words */ 3 * sizeof(long)> {
  using VB = VBase<BinaryBase, 3 * sizeof(long)>;
  using VB::VB;
  using VB::operator=;

  binspect::MMap owned_ {};
  binspect::MMap const& mm_;

  explicit Binary(binspect::MMap const& mm) : mm_(mm) { identify(); }
  explicit Binary(binspect::MMap&& mm) : owned_(std::move(mm)), mm_(owned_) { identify(); }

  void identify() {
    if (!mm_.ok()) { return; }
    void const* ptr = mm_.data_;
    if (!ptr) { return; }
    if (elf::ELF64LE::valid(ptr)) {
      assign(elf::ELF64LE(ptr));
    } else if (elf::ELF32LE::valid(ptr)) {
      assign(elf::ELF32LE(ptr));
    } else if (elf::ELF64BE::valid(ptr)) {
      assign(elf::ELF64BE(ptr));
    } else if (elf::ELF32BE::valid(ptr)) {
      assign(elf::ELF32BE(ptr));
    }
  }

  bool ok() const { return mm_.ok() && VB::ok(); }
};

}  // namespace binspect
