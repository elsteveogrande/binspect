#pragma once

#include <iostream>

namespace binspect {

struct error {
  int errno_;
  friend std::ostream& operator<<(std::ostream& os, error const& e) {
#if defined(__cpp_rtti)
    os << typeid(e).name() << ": ";
#else
    os << "error: ";
#endif
    return os;
  }
};

}  // namespace binspect
