#pragma once

#include "binspect/binary.h"
#include "binspect/memory.h"
#include "binspect/section.h"
#include "binspect/symbol.h"

#include <cstring>

namespace binspect {
namespace elf {

// Model classes for ELF structures.
// These support 64- and 32-bit layouts,
// either little- or big- endian.

template <class E, class Sec, class Sym>
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
    return self.template ptr<Sec>(self.shoff, i, self.shentsize);
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
    return {
        .vm_addr = shdr->addr,
        .name = {name_chars, strlen(name_chars)},
        .content = self.ptr(shdr->offset),
        .content_end = self.ptr(shdr->offset, shdr->size),
    };
  }

  res<section> find_section(this E const& self, std::string_view name) {
    for (auto sec : self.sections_view()) {
      if (sec.name == name) { return sec; }
    }
    return {name, ENOENT};
  }

  symbol __symbol(Sym const* sym, char const* names) const {
    auto* name_chars = names + sym->name_index;
    uint32_t flags = 0;
    switch ((sym->info >> 4) & 0x0f) {
    case 0:  flags |= uint32_t(symbol::flag::local); break;
    case 1:  flags |= uint32_t(symbol::flag::global); break;
    case 2:  flags |= uint32_t(symbol::flag::weak); break;
    default: break;
    }
    switch (sym->info & 0x0f) {
    case 1:  flags |= uint32_t(symbol::flag::data); break;
    case 2:  flags |= uint32_t(symbol::flag::code); break;
    default: flags |= uint32_t(symbol::flag::other); break;
    }
    return {
        .value = sym->value,
        .name = std::string_view(name_chars, strlen(name_chars)),
        .flags = flags,
        .size = sym->size};
  }

  view<symbol> symbols_view(this E const& self) {
    res<section> symtab;
    res<section> strtab;
    for (auto sec : self.sections_view()) {
      if (symtab->name.empty() && sec.name == ".symtab") {
        symtab = res<section>(std::move(sec));
      }
      if (strtab->name.empty() && sec.name == ".strtab") {
        strtab = res<section>(std::move(sec));
      }
      if (symtab->name.size() && strtab->name.size()) {
        auto const* syms = (Sym*) (symtab->content);
        auto const size = symtab->size();
        if (!(size % sizeof(Sym))) {
          auto count = size / sizeof(Sym);
          auto* names = (char const*) (strtab->content);
          return {.at = [=, &self](size_t i) { return self.__symbol(syms + i, names); },
                  .count = [=] { return count; }};
        }
      }
    }
    return view<symbol>::empty();
  }
};

}  // namespace elf

}  // namespace binspect
