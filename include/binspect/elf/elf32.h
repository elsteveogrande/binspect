#pragma once

#include "binspect/elf/base.h"
#include "binspect/words.h"

namespace binspect {
namespace elf {

template <class uint16_e, class uint32_e>
struct elf32 {
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
  uint32_e entry;      // Entry point virtual address
  uint32_e phoff;      // Program header table file offset
  uint32_e shoff;      // Section header table file offset
  uint32_e flags;      // Processor-specific flags
  uint16_e ehsize;     // ELF header size in bytes
  uint16_e phentsize;  // Program header table entry size
  uint16_e phnum;      // Program header table entry count
  uint16_e shentsize;  // Section header table entry size
  uint16_e shnum;      // Section header table entry count
  uint16_e shstrndx;   // Section header string table index

  struct section32 {
    uint32_e name_index;  // Section name (index into .shstrtab)
    uint32_e type;        // Section type
    uint32_e flags;       // Section flags
    uint32_e addr;        // Section virtual addr at execution
    uint32_e offset;      // Section file offset
    uint32_e size;        // Section size in bytes
    uint32_e link;        // Link to another section
    uint32_e info;        // Additional section information
    uint32_e addralign;   // Section alignment
    uint32_e entsize;     // Entry size if section holds table
  };
  static_assert(sizeof(section32) == 40);

  struct symbol32 {
    uint32_e name_index;  // Section name (index into .strtab)
    uint8_t info;         // Symbol type and binding
    uint8_t other;        // Symbol visibility
    uint16_e shndx;       // Section index [not reliable; ignore]
    uint32_e value;       // Symbol value
    uint32_e size;        // Symbol size
  };
  static_assert(sizeof(symbol32) == 16);
};

struct elf32le_base : elf32<__endians::uint16_le, __endians::uint32_le> {};
struct elf32le final
    : elf32le_base,
      __elf_base<elf32le, elf32le_base::section32, elf32le_base::symbol32> {
  bool valid() const {
    return magic == 0x464c457f && klass == 1 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32le) == 52);

struct elf32be_base : elf32<__endians::uint16_be, __endians::uint32_be> {};
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
