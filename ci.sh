#!/usr/bin/env bash

# Run `./ci.sh` from within project root dir

TESTCC=test/test_binspect.cc
BUILD=build/

set -xeuo pipefail

rm -rf build
mkdir build

clang++               \
  @compile_flags.txt  \
  -Og -g              \
  -o "${BUILD}/test"  \
  "${TESTCC}"

"${BUILD}/test" test/files/elf.64.le.exe

g++                   \
  @compile_flags.txt  \
  -Og -g              \
  -o "${BUILD}/test"  \
  "${TESTCC}"

"${BUILD}/test" test/files/elf.64.le.exe
