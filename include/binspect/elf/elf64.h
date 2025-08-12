#pragma once

#include "binspect/elf/base.h"
#include "binspect/words.h"

namespace binspect {
namespace elf {

template <class uint16_e, class uint32_e, class uint64_e>
struct elf64 {
  uint32_e magic;      // (7F 45 4C 46)
  uint8_t klass;       // (01) 32-bit, (02) 64-bit
  uint8_t endian;      // (01) LE, (02) BE; officially called `EI_DATA`
  uint8_t elfversion;  // (01)
  uint8_t osabi;       // we ignore this
  uint8_t abiversion;  // we ignore this
  uint8_t _pad[7];     //
  uint16_e type;       // Object file type // XXX worth adding an enum
  uint16_e machine;    // Architecture
  uint32_e version;    // (0x00000001) Object file version
  uint64_e entry;      // Entry point virtual address
  uint64_e phoff;      // Program header table file offset
  uint64_e shoff;      // Section header table file offset
  uint32_e flags;      // Processor-specific flags
  uint16_e ehsize;     // ELF header size in bytes
  uint16_e phentsize;  // Program header table entry size
  uint16_e phnum;      // Program header table entry count
  uint16_e shentsize;  // Section header table entry size
  uint16_e shnum;      // Section header table entry count
  uint16_e shstrndx;   // Section header string table index

  struct section64 {
    uint32_e name_index;  // Section name (index into .shstrtab)
    uint32_e type;        // Section type
    uint64_e flags;       // Section flags
    uint64_e addr;        // Section virtual addr at execution
    uint64_e offset;      // Section file offset
    uint64_e size;        // Section size in bytes
    uint32_e link;        // Link to another section
    uint32_e info;        // Additional section information
    uint64_e addralign;   // Section alignment
    uint64_e entsize;     // Entry size if section holds table
  };
  static_assert(sizeof(section64) == 64);

  struct symbol64 {
    uint32_e name_index;  // Section name (index into .strtab)
    uint8_t info;         // Symbol type and binding
    uint8_t other;        // Symbol visibility
    uint16_e shndx;       // Section index [not reliable; ignore]
    uint64_e value;       // Symbol value
    uint64_e size;        // Symbol size
  };
  static_assert(sizeof(symbol64) == 24);
};

struct elf64le_base
    : elf64<__endians::uint16_le, __endians::uint32_le, __endians::uint64_le> {};

struct elf64le final
    : elf64le_base,
      __elf_base<elf64le, elf64le_base::section64, elf64le_base::symbol64> {
  bool valid() const {
    return magic == 0x464c457f && klass == 2 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64le) == 64);

struct elf64be_base
    : elf64<__endians::uint16_be, __endians::uint32_be, __endians::uint64_be> {};

struct elf64be final
    : elf64be_base,
      __elf_base<elf64be, elf64be_base::section64, elf64be_base::symbol64> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 2 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64be) == 64);

}  // namespace elf

}  // namespace binspect
