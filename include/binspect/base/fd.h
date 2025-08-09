#pragma once

#include <binspect/base/error.h>
#include <cstddef>
#include <cstring>
#include <expected>
#include <fcntl.h>
#include <memory>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

namespace binspect {

struct fd;

namespace c {
enum class whence : int { from_begin = SEEK_SET, from_current = SEEK_CUR, from_end = SEEK_END };
std::expected<fd, error> open(std::string_view path, bool write = false);
std::expected<size_t, error> lseek(fd const& fd, off_t offset, whence whence);
}  // namespace c

struct fd {
  int fd_ {-1};
  operator bool() const { return fd_ != -1; }

  fd(int fd) : fd_(fd) {}
  fd(fd const&) = delete;
  fd& operator=(fd const&) = delete;
  operator int() const { return fd_; }

  fd(fd&& rhs) { std::swap(fd_, rhs.fd_); }
  fd& operator=(fd&& rhs) {
    return (std::addressof(rhs) == this) ? *this : *new (this) fd(std::move(rhs));
  }

  ~fd() {
    int fd {-1};
    std::swap(fd, fd_);
    if (fd > 0) { ::close(fd); }
  }

  static std::expected<fd, error> open(std::string_view path, bool write = false) {
    return c::open(path, write);
  }

  std::expected<size_t, error> to_end() { return lseek(*this, 0, c::whence::from_end); }
};

namespace c {
inline std::expected<fd, error> open(std::string_view path, bool write) {
  char path_buf[PATH_MAX + 1] {0};
  strncpy(path_buf, path.data(), sizeof(path_buf));
  int res = ::open(path_buf, write ? O_RDWR : O_RDONLY);
  if (res > 0) { return fd {res}; }
  return std::unexpected<error> {{errno}};
}

inline std::expected<size_t, error> lseek(fd const& fd, off_t offset, whence whence) {
  auto ret = ::lseek(fd, offset, int(whence));
  if (ret == (off_t) -1) { return std::unexpected {error {errno}}; }
  return ret;
}
}  // namespace c

}  // namespace binspect
