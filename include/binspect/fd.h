#pragma once

#include "binspect/error.h"

#include <cstddef>
#include <expected>
#include <fcntl.h>
#include <unistd.h>

namespace binspect {

namespace c {
std::expected<int, error> open(std::string_view path);
std::expected<off_t, error> lseek(int fileno, off_t offset, int whence);
}  // namespace c

struct fd {
  int fd_ {-1};
  off_t size_ {-1};
  operator bool() const { return fd_ > 0; }

  fd(int fd, off_t size = -1) : fd_(fd), size_(size) {}
  fd(fd const&) = delete;
  fd& operator=(fd const&) = delete;
  operator int() const { return fd_; }

  fd(fd&& rhs) {
    fd_ = rhs.fd_;
    size_ = rhs.size_;
    rhs.fd_ = -1;
    rhs.size_ = -1;
  }

  fd& operator=(fd&& rhs) {
    return (std::addressof(rhs) == this) ? *this : *new (this) fd(std::move(rhs));
  }

  ~fd() {
    int fd {-1};
    std::swap(fd, fd_);
    if (fd > 0) { ::close(fd); }
  }

  std::expected<size_t, error> size() const {
    if (size_ > 0) { return size_t(size_); }
    return std::unexpected {error {}};
  }

  static std::expected<fd, error> open(std::string_view path) {
    auto fileno = c::open(path);
    if (!fileno.has_value()) { return std::unexpected {fileno.error()}; }
    auto size = c::lseek(fileno.value(), 0, SEEK_END);
    if (!size.has_value()) { return std::unexpected {size.error()}; }
    return fd {fileno.value(), size.value()};
  }
};

namespace c {
inline std::expected<int, error> open(std::string_view path) {
  char path_buf[PATH_MAX + 1] {0};
  strncpy(path_buf, path.data(), sizeof(path_buf));
  int ret = ::open(path_buf, O_RDONLY);
  if (ret == -1) { return std::unexpected<error> {{errno}}; }
  return ret;
}

inline std::expected<off_t, error> lseek(int fileno, off_t offset, int whence) {
  auto ret = ::lseek(fileno, offset, whence);
  if (ret == -1) { return std::unexpected<error> {{errno}}; }
  return ret;
}

}  // namespace c

}  // namespace binspect
