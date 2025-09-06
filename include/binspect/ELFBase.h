// #pragma once
// static_assert(__cplusplus > 202300L, "binspect requires C++23");

// #include "binspect/Section.h"
// #include "binspect/Symbol.h"

// #include <cstring>
// #include <optional>

// namespace binspect {
// namespace elf {

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
//         .vm_addr = shdr->addr,
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
