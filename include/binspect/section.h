#pragma once

#include <format>
#include <ostream>
#include <print>
#include <string_view>

namespace binspect {

struct section final {
  uintptr_t vm_addr;
  std::string_view name;
  std::byte const* content;
  std::byte const* content_end;

  size_t size() const { return size_t(content_end - content); }

  friend std::ostream& operator<<(std::ostream& os, section const& self) {
    std::print(
        os,
        "(section vm_addr:0x{:012x} size:{} name:{})",
        self.vm_addr,
        (self.content_end - self.content),
        self.name);
    return os;
  }
};

}  // namespace binspect
