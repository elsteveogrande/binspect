#!/usr/bin/env bash

# Run `./ci.sh` from within project root dir

TESTCC=test/test_binspect.cc
BUILD=build/

set -xeuo pipefail

rm -rf build
mkdir build

clang++                         \
  @compile_flags.txt            \
  -O0 -g                        \
  -o "${BUILD}/test"            \
  -fsanitize=address,undefined  \
  -UNDEBUG                      \
  "${TESTCC}"

"${BUILD}/test" test/files/elf.64.le.exe
"${BUILD}/test" test/files/elf.64.le.so

"${BUILD}/test" test/files/elf.32.le.exe
"${BUILD}/test" test/files/elf.32.le.so

"${BUILD}/test" test/files/elf.64.be.exe
"${BUILD}/test" test/files/elf.32.be.exe
