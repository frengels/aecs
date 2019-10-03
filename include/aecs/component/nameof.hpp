#pragma once

#include <string_view>

#include "aecs/utility/priority_tag.hpp"

namespace aecs
{
namespace component
{
namespace detail
{
template<typename T>
constexpr auto name_impl(aecs::priority_tag<4>) noexcept(noexcept(T::name))
    -> decltype(T::name)
{
    return T::name;
}

// TODO add nameof
} // namespace detail

template<typename T>
struct nameof
{
    static constexpr std::string_view value =
        ::aecs::component::detail::name_impl<T>(aecs::max_priority_tag);
};

template<typename T>
inline constexpr std::string_view nameof_v = nameof<T>::value;
} // namespace component
} // namespace aecs
