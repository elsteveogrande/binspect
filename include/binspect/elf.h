#pragma once

#include <binspect/base/things.h>
#include <binspect/base/words.h>

namespace binspect {
namespace elf {

// Model classes for ELF structures.
// These support 64- and 32-bit layouts,
// either little- or big- endian.

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

  view<section> sections_view() const;

  struct segment32 {};
  struct section32 {};
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

  view<section> sections_view() const {
    return {.at = [this](size_t i) -> section { return section_at(i); },
            .count = [this]() -> size_t { return shnum; }};
  }

  // "Program Header" in ELF lingo
  struct segment64 {};

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

  template <typename T = std::byte>
  T const*
  ptr(size_t offset = 0, size_t item_index = 0, size_t item_size = sizeof(T)) const {
    return (T const*) (uintptr_t(this) + offset + (item_size * item_index));
  }

  section64 const* section_header_at(size_t i) const {
    return ptr<section64>(shoff, i, shentsize);
  }

  char const* string_table(size_t i) const {
    return ptr<char>(section_header_at(i)->offset);
  }

  section section_at(size_t i) const {
    auto st64 = section_header_at(i);
    auto strs = string_table(shstrndx);
    auto* name_chars = strs + st64->name_index;
    std::string_view name {name_chars, strlen(name_chars)};
    return {
        .vm_addr = st64->addr,
        .name = name,
        .content = ptr(st64->offset),
        .content_end = ptr(st64->offset, st64->size),
    };
  }
};

struct elf64le : elf64<__endians::u16le, __endians::u32le, __endians::u64le> {
  bool valid() const {
    return magic == 0x464c457f && klass == 2 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64le) == 64);
static_assert(sizeof(elf64le::section64) == 64);

struct elf64be : elf64<__endians::u16be, __endians::u32be, __endians::u64be> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 2 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf64be) == 64);

}  // namespace elf

}  // namespace binspect
