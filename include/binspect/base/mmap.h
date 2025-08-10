#pragma once

#include <binspect/base/fd.h>
#include <cassert>
#include <expected>
#include <sys/mman.h>

namespace binspect {

namespace c {
std::expected<std::byte*, error> mmap(fd const& fd);
}

struct mmap final {
  std::byte* addr_ {};
  size_t size_ {};

  ~mmap() {
    if (addr_) { munmap(addr_, size_); }
  }

  mmap(std::byte* addr, size_t size) : addr_(addr), size_(size) {}

  mmap(mmap const&) = delete;
  mmap& operator=(mmap const&) = delete;

  mmap(mmap&& rhs) {
    addr_ = rhs.addr_;
    size_ = rhs.size_;
    rhs.addr_ = 0;
    rhs.size_ = 0;
  }

  mmap& operator=(mmap&& rhs) {
    return (std::addressof(rhs) == this) ? *this : *new (this) mmap(std::move(rhs));
  }

  static std::expected<mmap, error> map_file(fd&& fd) {
    // Note: fd arg is "eaten" by this func, close after mmap is ok
    auto* addr = (std::byte*) c::mmap(fd).value();
    return mmap {addr, fd.size().value()};
  }
};

namespace c {
inline std::expected<std::byte*, error> mmap(fd const& fd) {
  auto* ret = ::mmap(nullptr, fd.size().value(), PROT_READ, MAP_SHARED, fd, 0);
  if (ret == MAP_FAILED) { return std::unexpected {error {errno}}; }
  return (std::byte*) ret;
}
}  // namespace c

}  // namespace binspect
