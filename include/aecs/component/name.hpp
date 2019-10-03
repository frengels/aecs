#pragma once

#include <string_view>

namespace aecs {
inline namespace cpo {
template <typename T> struct auto_fn {
  constexpr std::string_view operator()() const noexcept {}
};

template <typename T> inline constexpr auto name = name<T>{};
} // namespace cpo
} // namespace aecs
