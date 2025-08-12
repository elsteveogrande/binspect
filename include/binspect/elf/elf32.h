#pragma once

#include "binspect/elf/base.h"
#include "binspect/words.h"

namespace binspect {
namespace elf {

template <class u16, class u32>
struct elf32 {
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
  u32 entry;      // Entry point virtual address
  u32 phoff;      // Program header table file offset
  u32 shoff;      // Section header table file offset
  u32 flags;      // Processor-specific flags
  u16 ehsize;     // ELF header size in bytes
  u16 phentsize;  // Program header table entry size
  u16 phnum;      // Program header table entry count
  u16 shentsize;  // Section header table entry size
  u16 shnum;      // Section header table entry count
  u16 shstrndx;   // Section header string table index

  struct section32 {
    u32 name_index;  // Section name (index into .shstrtab)
    u32 type;        // Section type
    u32 flags;       // Section flags
    u32 addr;        // Section virtual addr at execution
    u32 offset;      // Section file offset
    u32 size;        // Section size in bytes
    u32 link;        // Link to another section
    u32 info;        // Additional section information
    u32 addralign;   // Section alignment
    u32 entsize;     // Entry size if section holds table
  };
  static_assert(sizeof(section32) == 40);

  struct symbol32 {
    u32 name_index;  // Section name (index into .strtab)
    u8 info;         // Symbol type and binding
    u8 other;        // Symbol visibility
    u16 shndx;       // Section index [not reliable; ignore]
    u32 value;       // Symbol value
    u32 size;        // Symbol size
  };
  static_assert(sizeof(symbol32) == 16);
};

struct elf32le_base : elf32<__endians::u16le, __endians::u32le> {};
struct elf32le final
    : elf32le_base,
      __elf_base<elf32le, elf32le_base::section32, elf32le_base::symbol32> {
  bool valid() const {
    return magic == 0x464c457f && klass == 1 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32le) == 52);

struct elf32be_base : elf32<__endians::u16be, __endians::u32be> {};
struct elf32be final
    : elf32be_base,
      __elf_base<elf32be, elf32be_base::section32, elf32be_base::symbol32> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 1 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32be) == 52);

}  // namespace elf

}  // namespace binspect
