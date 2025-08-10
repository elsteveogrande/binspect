#pragma once

#include "binspect/base/words.h"
#include "binspect/elf/base.h"

namespace binspect {
namespace elf {

template <class E, class u16, class u32, class u64>
struct elf64 : __elf_base<E> {
  u32 magic;      // (7F 45 4C 46)
  u8 klass;       // (01) 32-bit, (02) 64-bit
  u8 endian;      // (01) LE, (02) BE; officially called `EI_DATA`
  u8 elfversion;  // (01)
  u8 osabi;       // we ignore this
  u8 abiversion;  // we ignore this
  u8 _pad[7];     //
  u16 type;       // Object file type // XXX worth adding an enum
  u16 machine;    // Architecture
  u32 version;    // (0x00000001) Object file version
  u64 entry;      // Entry point virtual address
  u64 phoff;      // Program header table file offset
  u64 shoff;      // Section header table file offset
  u32 flags;      // Processor-specific flags
  u16 ehsize;     // ELF header size in bytes
  u16 phentsize;  // Program header table entry size
  u16 phnum;      // Program header table entry count
  u16 shentsize;  // Section header table entry size
  u16 shnum;      // Section header table entry count
  u16 shstrndx;   // Section header string table index

  struct section64 {
    u32 name_index;  // Section name (index into .shstrtab)
    u32 type;        // Section type
    u64 flags;       // Section flags
    u64 addr;        // Section virtual addr at execution
    u64 offset;      // Section file offset
    u64 size;        // Section size in bytes
    u32 link;        // Link to another section
    u32 info;        // Additional section information
    u64 addralign;   // Section alignment
    u64 entsize;     // Entry size if section holds table
  };

  using section_t = section64;
};

struct elf64le : elf64<elf64le, __endians::u16le, __endians::u32le, __endians::u64le> {
  bool valid() const {
    return magic == 0x464c457f && klass == 2 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64le) == 64);
static_assert(sizeof(elf64le::section64) == 64);

struct elf64be : elf64<elf64be, __endians::u16be, __endians::u32be, __endians::u64be> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 2 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64be) == 64);

}  // namespace elf

}  // namespace binspect
