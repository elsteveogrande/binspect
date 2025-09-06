#pragma once
#include <ranges>
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <format>
#include <print>
#include <string_view>

namespace binspect {

struct Section final {
  uintptr_t vm_addr;
  std::string_view name;
  std::byte const* content;
  std::byte const* content_end;

  size_t size() const { return size_t(content_end - content); }

  // friend std::ostream& operator<<(std::ostream& os, Section const& self) {
  //   auto* z = (void const*) &self;
  //   std::print("\nz:{:p}\n", z);
  //   auto a = self.vm_addr;
  //   std::print("a:{:p}\n", (void const*) a);
  //   auto b = self.content_end - self.content;
  //   std::print("b:{}\n", b);
  //   auto c = self.name;
  //   std::print("c:{:p},{}\n", (void const*) c.data(), c.size());
  //   std::print(os, "(section {:p} vm_addr:0x{:012x} size:{} name:{})", z, a,
  //   b, c); return os;
  // }
};

struct Sections final : std::ranges::view_interface<Section> {};

}  // namespace binspect
