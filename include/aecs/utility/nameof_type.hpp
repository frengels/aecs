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

template<typename T>
struct nameof_spacesaver
{
    static constexpr std::string_view dirty = nameof_type_impl<T>();

    static constexpr auto cleaner = [](auto strv, auto len) {
        std::array<char, decltype(len)::value + 1> arr{};

        for (auto i = 0; i < decltype(len)::value; ++i)
        {
            arr[i] = strv[i];
        }

        arr.back() = '\0';
        return arr;
    }(dirty, std::integral_constant<std::size_t, dirty.size()>{});

    static constexpr std::string_view clean = std::string_view{cleaner.data()};
};
} // namespace detail

template<typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept
{
    return detail::nameof_spacesaver<T>::clean;
}
} // namespace aecs