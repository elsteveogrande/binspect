#include <binspect/binspect.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>

int main() {
  std::allocator<std::byte> alloc;
  binspect::alloc_resource alloc_rsrc {alloc};
  binspect::heap heap {alloc_rsrc};
  binspect::context cx {heap};
  std::string_view path {"test/files/elf.64.le.exe"};
  auto fd = binspect::fd::open(path).value();
  std::cerr << "path:       " << path << '\n';
  std::cerr << "fd:         " << int(fd) << '\n';
  auto mm = binspect::mmap::map_file(std::move(fd)).value();
  std::cerr << "mmap addr:  " << mm.addr_ << '\n';
  std::cerr << "mmap size:  " << mm.size_ << '\n';
  auto bin = cx.binary_at(mm.addr_).value();
  std::cout << bin << '\n';
  for (auto section : bin.sections()) { std::cout << "... " << section << '\n'; }
  return 0;
}
