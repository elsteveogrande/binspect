[![CI](https://github.com/elsteveogrande/binspect/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/elsteveogrande/binspect/actions/workflows/ci.yml)

# binspect

## Binary Inspector

A library for reading executable binary formats.

* Executable programs as well as shared libraries
* ELF (64/32, and LE/BE) formats
* Mach-O formats

## Goals

* Simple API
* Async-signal safe operations throughout
* Compatibility and correctness

## Quick start

The only installation is:
* in your build, add an include path, e.g.: `-I/path/to/binspect/include`
* in your program, include `binspect/binspect.h`

Example minimal program:
(see also [test/test_binspect.cc](test/test_binspect.cc))

```
// Context object: contains things needed for your binspect sesh, mainly
// involving memory allocation.  We'll use the "default heap" for simplicity.
// (Not appropriate for signal-handler situations; see "async-signal" section below)
binspect::heap heap;
binspect::context cx {heap};

// Open, map, and interpret binary file.
// These return `std::expected`; program should actually check for errors
auto fd = binspect::fd::open(path);
auto mm = binspect::mmap::map_file(std::move(*fd));
auto bin = cx.binary_at(mm->addr_);

// Ready to do fun stuff with the opened binary!
std::cout << *bin << '\n';
for (auto section : bin->sections()) { std::cout << "... " << section << '\n'; }
```

## Discussion

### Async-signal-safety and allocations

`binspect` doesn't do anything that's not async-signal-safe, anticipating it might be used
during signal handling; e.g., collecting addresses in the stack frame and resolving those to
symbols and line numbers to print a stack trace.

This means your code calling `binspect` needs to provide a way to allocate stuff in memory
(so it can return things to you, or create transient temporary stuff needed internally).
All its allocations will ultimately go through the allocator object you pass to
the `context` constructor.  If you don't care about this, and the standard allocation
functions are okay to use in your situation, just use `std::allocator<std::byte>`.

The allocator you provide can be any type that provides `allocate` and `deallocate` functions
having the expected signatures:

```
std::byte* allocate(size_t n);
void deallocate(std::byte* p, size_t n);
```

This object is then wrapped in a `binspect::alloc_resource`, making it
a [memory resource](https://en.cppreference.com/w/cpp/memory/memory_resource.html).
You can provide such a memory resource as well; this `alloc_resource` class is a simple
adapter from the more-commonly-used "allocator-style" types to the type used
by `std::pmr`.


## TODO

* ELF symbols
* Optional cache
* Find companion debug info files (.dwo, .dSYM)
* Delegate calls for binary->{symbols, sections} to companions
* Mach-O segments
* Mach-O sections
* Mach-O symbols
* Collect all debug sections, in binary + companions (.debug_line, .dwo_debug_line, ...)


