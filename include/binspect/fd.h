#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <new>
#include <string_view>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

namespace binspect {

struct FD {
  int fd_ {};
  int errno_ {};

  ~FD() {
    if (ok()) {
      ::close(fd_);
      clear();
    }
  }

  FD() = default;

  FD(FD&& rhs) : fd_(rhs.fd_), errno_(rhs.errno_) { rhs.clear(); }

  FD& operator=(FD&& rhs) {
    if (&rhs != this) { new (this) FD(std::move(rhs)); }
    return *this;
  }

  FD(FD const&) = delete;
  FD& operator=(FD const&) = delete;

  explicit FD(int fd) : fd_(fd) {}

  explicit FD(char const* path) : FD(::open(path, O_RDONLY)) {
    if (!ok()) { errno_ = errno; }
  }

  explicit FD(std::string_view _path) {
    char path[1024];
    strncpy(path, _path.data(), sizeof(path));
    new (this) FD(path);
  }

  bool ok() const { return !(errno_ || fd_ < 0); }

  void clear() {
    fd_ = 0;
    errno_ = 0;
  }

  int operator*() const {
    assert(ok());
    return fd_;
  }

  struct stat stat() const {
    struct stat ret;
    memset(&ret, 0, sizeof(ret));
    ::fstat(fd_, &ret);
    return ret;
  }
};

}  // namespace binspect
