#pragma once

#include <binspect/base/things.h>
#include <binspect/base/words.h>

namespace binspect {
namespace elf {

// Model classes for ELF structures.
// These support 64- and 32-bit layouts,
// either little- or big- endian.

struct segment32 {};
struct section32 {};

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

  sections_view sections() const {}
  symbols_view symbols() const {}
};

struct elf32le : elf32<__endians::u16le, __endians::u32le> {
  bool valid() const {
    return magic == 0x464c457f && klass == 1 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32le) == 52);

struct elf32be : elf32<__endians::u16be, __endians::u32be> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 1 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32be) == 52);

struct segment64 {};
struct section64 {};

template <class u16, class u32, class u64>
struct elf64 {
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

  sections_view sections() const {}
  symbols_view symbols() const {}
};

struct elf64le : elf64<__endians::u16le, __endians::u32le, __endians::u64le> {
  bool valid() const {
    return magic == 0x464c457f && klass == 2 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64le) == 64);

struct elf64be : elf64<__endians::u16be, __endians::u32be, __endians::u64be> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 2 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64be) == 64);

}  // namespace elf

}  // namespace binspect
