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
    std::swap(fd_, rhs.fd_);
    std::swap(size_, rhs.size_);
  }

  fd& operator=(fd&& rhs) {
    return (std::addressof(rhs) == this) ? *this : *new (this) fd(std::move(rhs));
  }

  ~fd() {
    if (fd_ > 0) {
      ::close(fd_);
      fd_ = -1;
    }
  }

  std::expected<size_t, error> size() const {
    if (size_ > 0) { return size_t(size_); }
    return std::unexpected {error {}};
  }

  static std::expected<fd, error> open(std::string_view path) {
    auto fileno = c::open(path);
    if (!fileno) { return std::unexpected {fileno.error()}; }
    auto size = c::lseek(*fileno, 0, SEEK_END);
    if (!size) { return std::unexpected {size.error()}; }
    return fd {fileno.value(), *size};
  }
};

namespace c {
inline std::expected<int, error> open(std::string_view path) {
  char path_buf[PATH_MAX + 1] {0};
  strncpy(path_buf, path.data(), sizeof(path_buf));
  int ret = ::open(path_buf, O_RDONLY);
  if (ret == -1) { return std::unexpected<error> {error {}}; }
  return ret;
}

inline std::expected<off_t, error> lseek(int fileno, off_t offset, int whence) {
  auto ret = ::lseek(fileno, offset, whence);
  if (ret == -1) { return std::unexpected<error> {error {}}; }
  return ret;
}

}  // namespace c

}  // namespace binspect
