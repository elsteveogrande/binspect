#pragma once

#include <iostream>

namespace binspect {

struct error {
  int errno_ {-1};

  friend std::ostream& operator<<(std::ostream& os, error const& e) {
#if defined(__cpp_rtti) && __cpp_rtti >= 199711L
    os << typeid(e).name();
#else

    os << "error";
#endif
    os << ": [" << e.errno_ << "]" << ' ' << strerror(e.errno_);
    return os;
  }
};

}  // namespace binspect
