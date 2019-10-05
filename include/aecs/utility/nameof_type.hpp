#pragma once

#include <string_view>

namespace aecs
{
namespace detail
{
template<typename T>
[[nodiscard]] constexpr auto nameof_type_impl() noexcept
{
#if defined(__INTEL_COMPILER)
    constexpr auto n = std::string_view{__PRETTY_FUNCTION__ + 67,
                                        sizeof(__PRETTY_FUNCTION__) - 69};
#elif defined(__clang__)
    constexpr auto n = std::string_view{__PRETTY_FUNCTION__ + 43,
                                        sizeof(__PRETTY_FUNCTION__) - 45};
#elif defined(__GNUC__)
    constexpr auto n = std::string_view{__PRETTY_FUNCTION__ + 58,
                                        sizeof(__PRETTY_FUNCTION__) - 60};
#else
#error "Unsupported compiler"
#endif
    return n;
}
} // namespace detail

template<typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept
{
    return detail::nameof_type_impl<
        std::remove_cv_t<std::remove_reference_t<T>>>();
}
} // namespace aecs