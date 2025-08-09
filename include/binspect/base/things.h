#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <ranges>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace binspect {

struct context;

struct section {
  virtual ~section() = default;
  virtual std::string_view name() const = 0;
};

struct sections_view : std::ranges::view_interface<section> {
  virtual ~sections_view() = default;
};

struct symbol {
  virtual ~symbol() = default;
  virtual std::string_view name() const = 0;
  virtual uintptr_t value() const = 0;
};

struct symbols_view : std::ranges::view_interface<symbol> {
  virtual ~symbols_view() = default;
};

struct binary final : std::enable_shared_from_this<binary> {
  std::function<sections_view()> const sections;
  std::function<symbols_view()> const symbols;

  binary(std::function<sections_view()> sections, std::function<symbols_view()> symbols)
      : sections(std::move(sections))
      , symbols(std::move(symbols)) {}
};

using binary_ref = std::shared_ptr<binary>;

}  // namespace binspect
