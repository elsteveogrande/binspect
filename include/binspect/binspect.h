#pragma once
static_assert(__cplusplus > 202300L, "binspect requires C++23");
#include <version>
static_assert(__cpp_lib_memory_resource > 201600, "requires pmr types");

#include <binspect/base.h>
#include <binspect/elf.h>
