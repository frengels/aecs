#pragma once

#include <vector>

#include "aecs/utility/priority_tag.hpp"

namespace aecs
{
namespace component
{
namespace make_container_impl
{
template<typename T>
constexpr auto make_container(aecs::priority_tag<4>) noexcept(
    noexcept(T::make_container())) -> decltype(T::make_container())
{
    return T::make_container();
}

template<typename T>
constexpr auto make_container(aecs::priority_tag<3>) noexcept(noexcept(
    typename T::container_type{})) -> decltype(typename T::container_type{})
{
    return typename T::container_type{};
}

template<typename T>
constexpr std::vector<T> make_container(aecs::priority_tag<0>) noexcept(
    std::is_nothrow_constructible_v<std::vector<T>>)
{
    return {};
}
} // namespace make_container_impl

inline namespace cpo
{
template<typename T>
struct make_container_fn
{
    constexpr auto operator()() const noexcept(
        noexcept(::aecs::component::make_container_impl::make_container<T>(
            max_priority_tag)))
        -> decltype(::aecs::component::make_container_impl::make_container<T>(
            max_priority_tag))
    {
        return ::aecs::component::make_container_impl::make_container<T>(
            max_priority_tag);
    }
};

template<typename T>
inline constexpr auto make_container = make_container_fn<T>{};
} // namespace cpo
} // namespace component
} // namespace aecs