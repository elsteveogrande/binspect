#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/BinaryBase.h"
#include "binspect/Section.h"
#include "binspect/Symbol.h"
#include "binspect/Words.h"

#include <cstdint>
#include <cstring>
#include <span>

namespace binspect::elf {

template <class E, class ELong>
struct HeaderBase {
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

enum class ELFSecType : uint32_t {
  k_null = 0,
  k_progbits,
  k_symtab,
  k_strtab,
  k_rela,
  k_hash,
  k_dynamic,
  k_note,
  k_nobits,
  k_rel,
  k_shlib,
  k_dynsym,
  k_0c,
  k_0d,
  k_initarray,
  k_finiarray,
  k_preinitarray,
  k_group,
  k_symtab_shndx
};

template <class E, class ELong>
struct ELFSecBase {
  E::U32 name_index;  // Section name (index into .shstrtab)
  E::U32 type_;       // Section type
  ELong flags;        // Section flags
  ELong addr;         // Section virtual addr at execution
  ELong offset;       // Section file offset
  ELong size;         // Section size in bytes
  E::U32 link;        // Link to another section
  E::U32 info;        // Additional section information
  ELong addralign;    // Section alignment
  ELong entsize;      // Entry size if section holds table

  ELFSecType type() const { return ELFSecType(unsigned(type_)); }
};

template <class E, class ELong>
struct ELFSymBase {
  E::U32 name_index;  // Section name (index into .strtab)
  uint8_t info;       // Symbol type and binding
  uint8_t other;      // Symbol visibility
  E::U16 shndx;       // Section index
  ELong value;        // Symbol value
  ELong size;         // Symbol size
};

struct ELF : BinaryBase {
  uint16_t const shstrIndex_ {};
  uint16_t mutable symtabIndex_ {};
  uint16_t mutable strtabIndex_ {};

  ELF(uint16_t shstrIndex) : shstrIndex_(shstrIndex) {}

  virtual std::byte const* base() const = 0;

  std::string_view secNames(this auto const& self) {
    auto sec = self.secs()[self.shstrIndex_];
    auto* content = reinterpret_cast<char const*>(uintptr_t(self.base()) + sec.offset);
    return {content, sec.size};
  }

  std::string_view secName(this auto const& self, size_t i) {
    auto* data = self.secNames().data();
    auto sec = self.secs()[i];
    auto idx = sec.name_index;
    auto* ret = data + idx;
    return {ret};
  }

  bool findSymSections(this auto const& self) {
    if (self.symtabIndex_ && self.strtabIndex_) { return true; }
    uint16_t i = 0;
    for (auto& sec : self.secs()) {
      if (!self.symtabIndex_ && sec.type() == ELFSecType::k_symtab &&
          self.secName(i) == ".symtab") {
        self.symtabIndex_ = i;
      }
      if (!self.strtabIndex_ && sec.type() == ELFSecType::k_strtab &&
          self.secName(i) == ".strtab") {
        self.strtabIndex_ = i;
      }
      if (self.symtabIndex_ && self.strtabIndex_) { break; }
      ++i;
    }
    return self.symtabIndex_ && self.strtabIndex_;
  }

  std::string_view symNames(this auto const& self) {
    if (!self.symtabIndex_ || !self.strtabIndex_) { self.findSymSections(); }
    if (!self.symtabIndex_ || !self.strtabIndex_) { return ""; }
    auto sec = self.secs()[self.strtabIndex_];
    auto* content = reinterpret_cast<char const*>(uintptr_t(self.base()) + sec.offset);
    return {content, sec.size};
  }

  Section convertSec(this auto const& self, auto const& sec) {
    auto* names = self.secNames().data();
    auto* content = reinterpret_cast<std::byte*>(uintptr_t(self.base()) + sec.offset);
    return {
        .addr = sec.addr,
        .name = names + sec.name_index,
        .content = content,
        .contentEnd = content + sec.size,
    };
  }

  static Symbol::Binding _binding(uint8_t info) {
    switch (info >> 4) {
    case 0: return Symbol::Binding::LOCAL;
    case 1: return Symbol::Binding::GLOBAL;
    case 2: return Symbol::Binding::WEAK;
    }
    return Symbol::Binding::OTHER;
  }

  static Symbol::Type _type(uint8_t info) {
    switch (info & 0x0f) {
    case 1: return Symbol::Type::DATA;
    case 2: return Symbol::Type::CODE;
    case 3: return Symbol::Type::SECTION;
    case 4: return Symbol::Type::FILE;
    }
    return Symbol::Type::OTHER;
  }

  static Symbol::Visibility _visibility(uint8_t other) {
    switch (other) {
    case 0: return Symbol::Visibility::DEFAULT;
    case 2: return Symbol::Visibility::HIDDEN;
    case 3: return Symbol::Visibility::PROTECTED;
    }
    return Symbol::Visibility::OTHER;
  }

  Symbol convertSym(this auto const& self, auto const& sym) {
    return {
        .value = sym.value,
        .name = self.symNames().data() + sym.name_index,
        .binding = _binding(sym.info),
        .type = _type(sym.info),
        .visibility = _visibility(sym.other),
        .size = sym.size,
    };
  }

  Gen<Section> genSections(this auto& self) {
    for (auto& sec : self.secs()) { co_yield self.convertSec(sec); }
  }

  Gen<Symbol> genSymbols(this auto& self) {
    for (auto& sym : self.syms()) { co_yield self.convertSym(sym); }
  }
};

template <Endian E>
struct ELF64 : ELF {
  using Header = HeaderBase<E, typename E::U64>;
  using ELFSec = ELFSecBase<E, typename E::U64>;
  using ELFSym = ELFSymBase<E, typename E::U64>;
  static_assert(sizeof(Header) == 64);
  static_assert(sizeof(ELFSec) == 64);
  static_assert(sizeof(ELFSym) == 24);

  Header const* header_;
  explicit ELF64(Header const* header) : ELF {header->shstrndx}, header_ {header} {}
  explicit ELF64(void const* ptr) : ELF64 {reinterpret_cast<Header const*>(ptr)} {}
  std::byte const* base() const override { return reinterpret_cast<std::byte const*>(header_); }
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

  std::span<ELFSym> syms(this auto const& self) {
    if (!self.findSymSections()) { return {}; }
    auto sec = self.section(self.symtabIndex_);
    auto* begin = reinterpret_cast<ELFSym const*>(sec.content);
    auto* end = reinterpret_cast<ELFSym const*>(sec.contentEnd);
    return {const_cast<ELFSym*>(begin), end};
  }

  Section section(uint16_t i) const override { return convertSec(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }

  Gen<Symbol> symbols() const override { return genSymbols(); }
};

struct ELF64BE final : ELF64<BE> {
  using ELF64::ELF64;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF64<BE>::Header const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 2 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const {
    auto addr = uintptr_t(base()) + header_->shoff;
    assert(sizeof(ELFSec) == header_->shentsize);
    return std::span<ELFSec>(reinterpret_cast<ELFSec*>(addr), size_t(header_->shnum));
  }

  std::span<ELFSym> syms(this auto const& self) {
    if (!self.findSymSections()) { return {}; }
    auto sec = self.section(self.symtabIndex_);
    auto* begin = reinterpret_cast<ELFSym const*>(sec.content);
    auto* end = reinterpret_cast<ELFSym const*>(sec.contentEnd);
    return {const_cast<ELFSym*>(begin), end};
  }

  Section section(uint16_t i) const override { return convertSec(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }

  Gen<Symbol> symbols() const override { return genSymbols(); }
};

template <Endian E>
struct ELF32 : ELF {
  using Header = HeaderBase<E, typename E::U32>;
  using ELFSec = ELFSecBase<E, typename E::U32>;
  using ELFSym = ELFSymBase<E, typename E::U32>;
  static_assert(sizeof(Header) == 52);
  static_assert(sizeof(ELFSec) == 40);
  static_assert(sizeof(ELFSym) == 16);

  Header const* header_;
  explicit ELF32(Header const* header) : ELF {header->shstrndx}, header_ {header} {}

  explicit ELF32(void const* ptr) : ELF32 {reinterpret_cast<Header const*>(ptr)} {}
  std::byte const* base() const override { return reinterpret_cast<std::byte const*>(header_); }
};

struct ELF32LE final : ELF32<LE> {
  using ELF32::ELF32;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF32<LE>::Header const*>(ptr);
    return h.magic == 0x464c457f  // LE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 1 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const {
    auto addr = uintptr_t(base()) + header_->shoff;
    assert(sizeof(ELFSec) == header_->shentsize);
    return std::span<ELFSec>(reinterpret_cast<ELFSec*>(addr), size_t(header_->shnum));
  }

  std::span<ELFSym> syms(this auto const& self) {
    if (!self.findSymSections()) { return {}; }
    auto sec = self.section(self.symtabIndex_);
    auto* begin = reinterpret_cast<ELFSym const*>(sec.content);
    auto* end = reinterpret_cast<ELFSym const*>(sec.contentEnd);
    return {const_cast<ELFSym*>(begin), end};
  }

  Section section(uint16_t i) const override { return convertSec(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }

  Gen<Symbol> symbols() const override { return genSymbols(); }
};

struct ELF32BE final : ELF32<BE> {
  using ELF32::ELF32;

  static bool valid(void const* ptr) {
    auto& h = *reinterpret_cast<ELF32<BE>::Header const*>(ptr);
    return h.magic == 0x7f454c46  // BE 32-bit read of bytes (7F 45 4C 46)
           && h.klass == 1 && h.endian == 2 && h.elfversion == 1 && h.version == 1;
  }

  std::span<ELFSec> secs() const {
    auto addr = uintptr_t(base()) + header_->shoff;
    assert(sizeof(ELFSec) == header_->shentsize);
    return std::span<ELFSec>(reinterpret_cast<ELFSec*>(addr), size_t(header_->shnum));
  }

  std::span<ELFSym> syms(this auto const& self) {
    if (!self.findSymSections()) { return {}; }
    auto sec = self.section(self.symtabIndex_);
    auto* begin = reinterpret_cast<ELFSym const*>(sec.content);
    auto* end = reinterpret_cast<ELFSym const*>(sec.contentEnd);
    return {const_cast<ELFSym*>(begin), end};
  }

  Section section(uint16_t i) const override { return convertSec(secs()[i]); }
  Gen<Section> sections() const override { return genSections(); }

  Gen<Symbol> symbols() const override { return genSymbols(); }
};

}  // namespace binspect::elf
