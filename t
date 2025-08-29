#!/usr/bin/env bash

if [[ ! -d include ]] ; then
  echo "chdir to project root, then run as: scripts/pre"
  exit 1
fi

set -euo pipefail

export COMP=${COMP:-llvm}
export OPT=${OPT:--O0 -fno-inline -fsanitize=address,undefined}
export TESTCC=${TESTCC:-test/TestBinary.cc}
export PROG=$(echo $TESTCC | sed 's#test/#build/#' | sed 's/.cc$/.cc.exe/')

echo "COMP    ${COMP}"
echo "OPT     ${OPT}"
echo "TESTCC  ${TESTCC}"
echo "PROG    ${PROG}"

scripts/buildtest
$PROG test/files/elf.64.le.exe

# Automated cleanups + checks
find include/ test/ -type f -name \*.h -o -name \*\.cc \
  | xargs clang-format -i
