#pragma once

#include "binspect/things.h"

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

}  // namespace elf

}  // namespace binspect
