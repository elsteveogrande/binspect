#include <binspect/binspect.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

int main(int argc, char** argv) {
  assert(argc == 2);
  std::allocator<std::byte> alloc;
  binspect::alloc_resource alloc_rsrc {alloc};
  binspect::heap heap {alloc_rsrc};
  binspect::context cx {heap};
  std::string_view path {argv[1]};
  auto fd = binspect::fd::open(path);
  assert(fd.has_value());
  std::cerr << "path:       " << path << '\n';
  std::cerr << "fd:         " << int(fd.value()) << '\n';
  auto mm = binspect::mmap::map_file(std::move(fd.value()));
  assert(mm.has_value());
  std::cerr << "mmap addr:  " << mm.value().addr_ << '\n';
  std::cerr << "mmap size:  " << mm.value().size_ << '\n';
  auto bin = cx.binary_at(mm.value().addr_);
  assert(bin.has_value());
  std::cout << bin.value() << '\n';
  for (auto section : bin.value().sections()) { std::cout << "... " << section << '\n'; }
  return 0;
}
