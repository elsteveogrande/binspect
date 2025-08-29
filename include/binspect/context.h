#pragma once

#include "binspect/binary.h"
#include "binspect/context-decl.h"
#include "binspect/elf/elf32.h"
#include "binspect/elf/elf64.h"
#include "binspect/memory.h"

namespace binspect {

struct binary;

inline res<binary> context::binary_at(void const* ptr) {
  if (ptr) {
    {
      auto* elf = (elf::elf64le const*) ptr;
      if (elf->valid()) {
        return res<binary>(
            [elf]() { return elf->sections_view(); },
            [elf]() { return elf->symbols_view(); });
      }
    }
    {
      auto* elf = (elf::elf32le const*) ptr;
      if (elf->valid()) {
        return res<binary>(
            [elf]() { return elf->sections_view(); },
            [elf]() { return elf->symbols_view(); });
      }
    }
    {
      auto* elf = (elf::elf64be const*) ptr;
      if (elf->valid()) {
        return res<binary>(
            [elf]() { return elf->sections_view(); },
            [elf]() { return elf->symbols_view(); });
      }
    }
    {
      auto* elf = (elf::elf32be const*) ptr;
      if (elf->valid()) {
        return res<binary>(
            [elf]() { return elf->sections_view(); },
            [elf]() { return elf->symbols_view(); });
      }
    }
  }
  return {error(-1)};
}

}  // namespace binspect
