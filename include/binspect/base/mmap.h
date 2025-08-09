#pragma once

#include "binspect/base/fd.h"

#include <expected>
#include <memory>
#include <sys/mman.h>

namespace binspect {

namespace c {
std::expected<std::byte*, error> mmap();
}

struct mmap : std::enable_shared_from_this<mmap> {
  std::byte* addr_;
  size_t size_;

  ~mmap() { munmap(addr_, size_); }
  mmap(std::byte* addr, size_t size) : addr_(addr), size_(size) {}

  mmap(mmap const&) = delete;
  mmap(mmap&&) = default;
  mmap& operator=(mmap const&) = delete;
  mmap& operator=(mmap&&) = default;

  static std::expected<mmap, error> map_file(fd&& fd) {
    auto size = fd.to_end().value();
    auto* addr = (std::byte*) c::mmap().value();
    return mmap {addr, size};
  }
};

namespace c {
inline std::expected<std::byte*, error> mmap(fd const& fd, size_t size) {
  auto* ret = ::mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
  if (ret == MAP_FAILED) { return std::unexpected {error {errno}}; }
  return (std::byte*) ret;
}
}  // namespace c

}  // namespace binspect
