# binspect

## Binary Inspector

A library for navigating executable formats.

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

## TODO

* Build a C++20 module from `binspect.h`
