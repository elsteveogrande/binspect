#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");

#include "binspect/FD.h"

#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

namespace binspect {

struct MMap {
  FD fd_ {};
  void* data_ {};
  size_t size_ {};
  int errno_ {};

  MMap() = default;

  MMap(MMap&& rhs)
      : fd_(std::move(rhs.fd_))
      , data_(rhs.data_)
      , size_(rhs.size_)
      , errno_(rhs.errno_) {
    clear();
  }

  MMap& operator=(MMap&& rhs) {
    if (&rhs != this) { new (this) MMap(std::move(rhs)); }
    return *this;
  }

  MMap(MMap const&) = delete;
  MMap& operator=(MMap const&) = delete;

  ~MMap() {
    if (ok()) {
      ::munmap(data_, size_);
      clear();
    }
  }

  bool ok() const { return !errno_ && data_ && size_; }

  void clear() {
    data_ = nullptr;
    size_ = 0;
    errno_ = 0;
  }

  explicit MMap(FD&& fd) : fd_(std::move(fd)) {
    if (fd_.ok()) {
      size_ = static_cast<size_t>(fd_.stat().st_size);
      if (!size_) {
        errno_ = errno;
      } else {
        data_ = ::mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, *fd_, 0);
        if (intptr_t(data_) == -1) { errno_ = errno; }
      }
    }
    if (errno_) { clear(); }
  }
};

}  // namespace binspect
