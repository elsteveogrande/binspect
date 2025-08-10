#pragma once

#include <binspect/base/view.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <ostream>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace binspect {

struct context;

struct section final {
  uintptr_t vm_addr;
  std::string_view name;
  std::byte const* content;
  std::byte const* content_end;

  friend std::ostream& operator<<(std::ostream& os, section const& self) {
    return os << "(section"
              << " vm_addr:" << ((void*) self.vm_addr) << " name:" << self.name
              << " size:" << (self.content_end - self.content) << ")";
  }
};

// struct symbol {
//   virtual ~symbol() = default;
//   virtual std::string_view name() const = 0;
//   virtual uintptr_t value() const = 0;
// };

struct binary final {
  context& cx_;
  std::function<view<section>()> sections {};

  friend std::ostream& operator<<(std::ostream& os, binary const& self) {
    return os << "(binary"
              << " sectionCount:" << self.sections().count() << ")";
  }
};

}  // namespace binspect
