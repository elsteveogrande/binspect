#pragma once

#include <cassert>
#include <cstring>
#include <iostream>

namespace binspect {

struct error {
  std::string_view msg_ {};
  int errno_;

  error(int errno_) : errno_(errno) { assert(errno_ != 0); }
  error(std::string_view msg) : error(msg, -1) {}
  error(std::string_view msg, int errno_) : msg_(msg), errno_(errno_) {}

  friend std::ostream& operator<<(std::ostream& os, error const& e) {
#if defined(__cpp_rtti) && __cpp_rtti >= 199711L
    os << typeid(e).name();
#else
    os << "error";
#endif
    os << ": [" << e.errno_ << "]" << ' ' << strerror(e.errno_);
    if (!e.msg_.empty()) { os << ' ' << e.msg_; }
    return os;
  }
};

}  // namespace binspect
