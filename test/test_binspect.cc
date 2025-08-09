#include <binspect/binspect.h>

int main(int argc, char** argv) {
  std::allocator<std::byte> alloc;
  binspect::alloc_resource alloc_rsrc {alloc};
  binspect::heap heap {alloc_rsrc};
  binspect::context cx {heap};
  std::string_view path(argv[argc - 1]);
  auto fd = binspect::fd::open(path).value();
  auto mm = binspect::mmap::map_file(std::move(fd)).value();
  auto bin = cx.binary_at(mm.addr_);
  return 0;
}
