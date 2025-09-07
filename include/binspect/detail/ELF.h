#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/Section.h"
// #include "binspect/Symbol.h"
#include "binspect/Words.h"

#include <cstdint>
#include <cstring>
#include <span>

namespace binspect::elf {

template <class E, class ELong>
struct Header {
  E::U32 magic;        // (7F 45 4C 46)
  uint8_t klass;       // (01) 32-bit, (02) 64-bit
  uint8_t endian;      // (01) LE, (02) BE; officially called `EI_DATA`
  uint8_t elfversion;  // (01)
  uint8_t osabi;       // we ignore this
  uint8_t abiversion;  // we ignore this
  uint8_t _pad[7];     //
  E::U16 type;         // Object file type // XXX worth adding an enum
  E::U16 machine;      // Architecture
  E::U32 version;      // (0x00000001) Object file version
  ELong entry;         // Entry point virtual address
  ELong phoff;         // Program header table file offset
  ELong shoff;         // Section header table file offset
  E::U32 flags;        // Processor-specific flags
  E::U16 ehsize;       // ELF header size in bytes
  E::U16 phentsize;    // Program header table entry size
  E::U16 phnum;        // Program header table entry count
  E::U16 shentsize;    // Section header table entry size
  E::U16 shnum;        // Section header table entry count
  E::U16 shstrndx;     // Section header string table index
};

template <class E, class ELong>
struct ELFSec {
  E::U32 name_index;  // Section name (index into .shstrtab)
  E::U32 type;        // Section type
  ELong flags;        // Section flags
  ELong addr;         // Section virtual addr at execution
  ELong offset;       // Section file offset
  ELong size;         // Section size in bytes
  E::U32 link;        // Link to another section
  E::U32 info;        // Additional section information
  ELong addralign;    // Section alignment
  ELong entsize;      // Entry size if section holds table
};

template <class E, class ELong>
struct ELFSym {
  E::U32 name_index;  // Section name (index into .strtab)
  uint8_t info;       // Symbol type and binding
  uint8_t other;      // Symbol visibility
  E::U16 shndx;       // Section index [not reliable; ignore]
  ELong value;        // Symbol value
  ELong size;         // Symbol size
};

struct ELF : BinaryBase {
  uint16_t const shstrIndex_ {};

  explicit ELF(uint16_t shstrIndex) : shstrIndex_(shstrIndex) {}

  // int16_t mutable symtabIndex_ {};
  // int16_t mutable strtabIndex_ {};

  // std::string_view symNames() const {
  //   auto symNames = sections()[*index];
  //   return {reinterpret_cast<char const*>(symNames.content),
  //           reinterpret_cast<char const*>(symNames.contentEnd)};
  // }

  virtual void const* base() const = 0;

  // std::span<ELFSec> secs() const {}

  std::string_view secNames(this auto const& self) {
    auto sec = self.secs()[self.shstrIndex_];
    auto* content = reinterpret_cast<char const*>(uintptr_t(self.base()) + sec.offset);
    return {content, sec.size};
  }

  Section convert(this auto const& self, auto const& sec) {
    auto* content = (std::byte*) (uintptr_t(self.base()) + sec.offset);
    return {
        .addr = sec.addr,
        .name = self.secNames().data() + sec.name_index,
        .content = content,
        .contentEnd = content + sec.size,
    };
  }

  Gen<Section> genSections(this auto& self) {
    for (auto& sec : self.secs()) { co_yield self.convert(sec); }
  }
};

template <Endian E>
struct ELF64 : ELF {
  using Header = Header<E, typename E::U64>;
  using ELFSec = ELFSec<E, typename E::U64>;
  using ELFSym = ELFSym<E, typename E::U64>;
  static_assert(sizeof(Header) == 64);
  static_assert(sizeof(ELFSec) == 64);
  static_assert(sizeof(ELFSym) == 24);

  Header const* header_;
  explicit ELF64(Header const* header) : ELF {header->shstrndx}, header_ {header} {}
  explicit ELF64(void const* ptr) : ELF64 {reinterpret_cast<Header const*>(ptr)} {}
  void const* base() const override { return header_; }
};

struct ELF64LE final : ELF64<LE> {
  using ELF64::ELF64;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF64<LE>::Header const*>(ptr);
    return h.magic == 0x464c457f  // LE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 2 && h.endian == 1 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const {
    auto addr = uintptr_t(base()) + header_->shoff;
    assert(sizeof(ELFSec) == header_->shentsize);
    return std::span<ELFSec>(reinterpret_cast<ELFSec*>(addr), size_t(header_->shnum));
  }

  Section section(uint16_t i) const override { return convert(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }
};

struct ELF64BE final : ELF64<BE> {
  using ELF64::ELF64;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF64<BE>::Header const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 2 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const { return std::span<ELFSec>(); }

  Section section(uint16_t i) const override { return convert(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }
};

template <Endian E>
struct ELF32 : ELF {
  using Header = Header<E, typename E::U32>;
  using ELFSec = ELFSec<E, typename E::U32>;
  using ELFSym = ELFSym<E, typename E::U32>;
  static_assert(sizeof(Header) == 52);
  static_assert(sizeof(ELFSec) == 40);
  static_assert(sizeof(ELFSym) == 16);

  Header const* header_;
  explicit ELF32(Header const* header) : ELF {header->shstrndx}, header_ {header} {}
  explicit ELF32(void const* ptr) : ELF32 {reinterpret_cast<Header const*>(ptr)} {}
  void const* base() const override { return header_; }
};

struct ELF32LE final : ELF32<LE> {
  using ELF32::ELF32;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF32<LE>::Header const*>(ptr);
    return h.magic == 0x464c457f  // LE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 1 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const { return std::span<ELFSec>(); }

  Section section(uint16_t i) const override { return convert(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }
};

struct ELF32BE final : ELF32<BE> {
  using ELF32::ELF32;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF32<BE>::Header const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const { return std::span<ELFSec>(); }

  Section section(uint16_t i) const override { return convert(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }
};

}  // namespace binspect::elf

// // Model classes for ELF structures.
// // These support 64- and 32-bit layouts,
// // either little- or big- endian.

// template <class E, class Sec, class Sym>
// struct ELFBase {

//   template <typename T = std::byte>
//   T const*
//   ptr(this E const& self,
//       size_t offset = 0,
//       size_t item_index = 0,
//       size_t item_size = sizeof(T)) {
//     return (T const*) (uintptr_t(&self) + offset + (item_size * item_index));
//   }

//   auto const* section_header_at(this E const& self, size_t i) {
//     return self.template ptr<Sec>(self.shoff, i, self.shentsize);
//   }

//   char const* string_table(this E const& self, size_t i) {
//     return self.template ptr<char>(self.section_header_at(i)->offset);
//   }

//   Sections sections_view(this E const& self) {
//     return {
//         .at_ = [&self](size_t i) -> Section {
//           std::print("@@@ i: {}\n", i);
//           return self.section_at(i);
//         },
//         .count_ = [&self]() -> size_t { return self.shnum; }};
//   }

//   Section section_at(this E const& self, size_t i) {
//     auto shdr = self.section_header_at(i);
//     auto strs = self.string_table(self.shstrndx);
//     auto* name_chars = strs + shdr->name_index;
//     std::print(
//         "@@@ section_at i={} name={} idx={}\n",
//         i,
//         name_chars,
//         size_t(shdr->name_index));
//     return {
//         .addr = shdr->addr,
//         .name = {name_chars, strlen(name_chars)},
//         .content = self.ptr(shdr->offset),
//         .content_end = self.ptr(shdr->offset, shdr->size),
//     };
//   }

//   std::optional<Section> find_section(this E const& self, std::string_view
//   name) {
//     for (auto sec : self.sections_view()) {
//       if (sec.name == name) { return sec; }
//     }
//     return {};
//   }

//   Symbol __symbol(Sym const* sym, char const* names) const {
//     auto* name_chars = names + sym->name_index;
//     uint32_t flags = 0;
//     switch ((sym->info >> 4) & 0x0f) {
//     case 0:  flags |= uint32_t(Symbol::Flag::local); break;
//     case 1:  flags |= uint32_t(Symbol::Flag::global); break;
//     case 2:  flags |= uint32_t(Symbol::Flag::weak); break;
//     default: break;
//     }
//     switch (sym->info & 0x0f) {
//     case 1:  flags |= uint32_t(Symbol::Flag::data); break;
//     case 2:  flags |= uint32_t(Symbol::Flag::code); break;
//     default: flags |= uint32_t(Symbol::Flag::other); break;
//     }
//     return {
//         .value = sym->value,
//         .name = std::string_view(name_chars, strlen(name_chars)),
//         .flags = flags,
//         .size = sym->size};
//   }

//   Symbols symbols_view(this E const& self) {
//     Section symtab;
//     Section strtab;
//     for (auto sec : self.sections_view()) {
//       if (symtab.name.empty() && sec.name == ".symtab") {
//         symtab = Section(std::move(sec));
//       }
//       if (strtab.name.empty() && sec.name == ".strtab") {
//         strtab = Section(std::move(sec));
//       }
//       if (symtab.name.size() && strtab.name.size()) {
//         auto const* syms = (Sym*) (symtab.content);
//         auto const size = symtab.size();
//         if (!(size % sizeof(Sym))) {
//           auto count = size / sizeof(Sym);
//           auto* names = (char const*) (strtab.content);
//           return {
//               .at_ =
//                   [=, &self](size_t i) { return self.__symbol(syms + i,
//                   names); },
//               .count_ = [=] { return count; }};
//         }
//       }
//     }
//     return {};
//   }
// };

// }  // namespace elf

// }  // namespace binspect
