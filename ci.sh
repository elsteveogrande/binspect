#!/usr/bin/env bash

# Run `./ci.sh` from within project root dir

CLANG=${CLANG:-clang++}

TESTCC=test/test_binspect.cc
BUILD=$(mktemp -d)

set -xeuo pipefail

rm -rf build
mkdir build

for OPT in O0 O1 Os Og O3 ; do
  for SAN in undefined address thread ; do
    PROG="${BUILD}/test_${OPT}_${SAN}"
    rm -f "${PROG}"
    "${CLANG}"                      \
      @compile_flags.txt            \
      -g                            \
      "-${OPT}"                     \
      -o "${PROG}"                  \
      -fsanitize="${SAN}"           \
      -UNDEBUG                      \
      "${TESTCC}"

    for FORMAT in elf ; do
      for BITS in 64 32 ; do
        for ENDIAN in le be ; do
          for TYPE in exe so ; do
            # Don't currently have big-endian shared libs
            if [[ "${FORMAT}-${ENDIAN}-${TYPE}" == "elf-be-so" ]]; then continue; fi
            BINFILE="test/files/${FORMAT}.${BITS}.${ENDIAN}.${TYPE}"
            "${PROG}" "${BINFILE}"
          done
        done
      done
    done
  done
done
