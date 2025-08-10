#pragma once

#include <binspect/base/things.h>
#include <binspect/base/words.h>

namespace binspect {
namespace elf {

// Model classes for ELF structures.
// These support 64- and 32-bit layouts,
// either little- or big- endian.

template <class E>
struct __elf_base {
  template <typename T = std::byte>
  T const*
  ptr(this E const& self,
      size_t offset = 0,
      size_t item_index = 0,
      size_t item_size = sizeof(T)) {
    return (T const*) (uintptr_t(&self) + offset + (item_size * item_index));
  }

  auto const* section_header_at(this E const& self, size_t i) {
    return self.template ptr<typename E::section_t>(self.shoff, i, self.shentsize);
  }

  char const* string_table(this E const& self, size_t i) {
    return self.template ptr<char>(self.section_header_at(i)->offset);
  }

  view<section> sections_view(this E const& self) {
    return {.at = [&self](size_t i) -> section { return self.section_at(i); },
            .count = [&self]() -> size_t { return self.shnum; }};
  }

  section section_at(this E const& self, size_t i) {
    auto shdr = self.section_header_at(i);
    auto strs = self.string_table(self.shstrndx);
    auto* name_chars = strs + shdr->name_index;
    std::string_view name {name_chars, strlen(name_chars)};
    return {
        .vm_addr = shdr->addr,
        .name = name,
        .content = self.ptr(shdr->offset),
        .content_end = self.ptr(shdr->offset, shdr->size),
    };
  }
};

template <class E, class u16, class u32>
struct elf32 : __elf_base<E> {
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
  using section_t = section32;
};

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

struct elf32le final : elf32<elf32le, __endians::u16le, __endians::u32le> {
  bool valid() const {
    return magic == 0x464c457f && klass == 1 && endian == 1 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32le) == 52);
static_assert(sizeof(elf32le::section32) == 40);

struct elf32be final : elf32<elf32be, __endians::u16be, __endians::u32be> {
  bool valid() const {
    return magic == 0x7f454c46 && klass == 1 && endian == 2 && elfversion == 1 && version == 1;
  }
};
static_assert(sizeof(elf32be) == 52);
static_assert(sizeof(elf32le::section32) == 40);

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
