#pragma once
#include "binspect/section.h"
#include "binspect/symbol.h"
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/Words.h"

namespace binspect {
namespace elf {

template <class uint16_e, class uint32_e, class uint64_e>
struct ELF64 {
  struct Header64 {
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
  };

  struct Section64 {
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

  struct Symbol64 {
    uint32_e name_index;  // Section name (index into .strtab)
    uint8_t info;         // Symbol type and binding
    uint8_t other;        // Symbol visibility
    uint16_e shndx;       // Section index [not reliable; ignore]
    uint64_e value;       // Symbol value
    uint64_e size;        // Symbol size
  };

  static_assert(sizeof(Header64) == 64);
  static_assert(sizeof(Section64) == 64);
  static_assert(sizeof(Symbol64) == 24);

  Header64 const* base_;
};

struct ELF64LEBase : ELF64<_Endians::U16LE, _Endians::U32LE, _Endians::U64LE> {
};

struct ELF64LE final : ELF64LEBase, BinaryBase {
  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<Header64 const*>(ptr);
    return h.magic == 0x464c457f  // LE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 2 && h.endian == 1 && h.elfversion == 1 && h.version == 1;
  }

  Header64 const* base_;

  ELF64LE(void const* ptr) : base_(reinterpret_cast<Header64 const*>(ptr)) {}

  Sections sections() const override {}

  Symbols symbols() const override {}
};

struct ELF64BEBase : ELF64<_Endians::U16BE, _Endians::U32BE, _Endians::U64BE> {
};

struct ELF64BE final : ELF64BEBase, BinaryBase {
  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<Header64 const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 2 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  Header64 const* base_;

  ELF64BE(void const* ptr) : base_(reinterpret_cast<Header64 const*>(ptr)) {}

  Sections sections() const override {}

  Symbols symbols() const override {}
};

}  // namespace elf

}  // namespace binspect
