#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/Words.h"

namespace binspect {
namespace elf {

template <class uint16_e, class uint32_e>
struct ELF32 {
  struct Header32 {
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
  };

  struct Section32 {
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

  struct Symbol32 {
    uint32_e name_index;  // Section name (index into .strtab)
    uint8_t info;         // Symbol type and binding
    uint8_t other;        // Symbol visibility
    uint16_e shndx;       // Section index [not reliable; ignore]
    uint32_e value;       // Symbol value
    uint32_e size;        // Symbol size
  };

  static_assert(sizeof(Header32) == 52);
  static_assert(sizeof(Section32) == 40);
  static_assert(sizeof(Symbol32) == 16);
};

struct ELF32LEBase : ELF32<_Endians::U16LE, _Endians::U32LE> {};
struct ELF32LE final : ELF32LEBase, BinaryBase {
  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<Header32 const*>(ptr);
    return h.magic == 0x464c457f  // LE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 1 && h.elfversion == 1 && h.version == 1;
  }

  Header32 const* base_;

  ELF32LE(void const* ptr) : base_(reinterpret_cast<Header32 const*>(ptr)) {}

  Sections sections() const override {}

  Symbols symbols() const override {}
};

struct ELF32BEBase : ELF32<_Endians::U16BE, _Endians::U32BE> {};
struct ELF32BE final : ELF32BEBase, BinaryBase {
  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<Header32 const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  Header32 const* base_;

  ELF32BE(void const* ptr) : base_(reinterpret_cast<Header32 const*>(ptr)) {}

  Sections sections() const override {}

  Symbols symbols() const override {}
};

}  // namespace elf

}  // namespace binspect
