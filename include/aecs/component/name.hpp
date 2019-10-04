#pragma once

#include <string_view>

#include "aecs/component/type_identity.hpp"
#include "aecs/utility/priority_tag.hpp"

namespace aecs
{
namespace component
{
namespace name_impl
{
template<typename T>
constexpr auto
    name(aecs::priority_tag<4>,
         aecs::component::type_identity<T>) noexcept(noexcept(std::string_view{
        T::name})) -> decltype(std::string_view{T::name})
{
    return std::string_view{T::name};
}

template<typename T>
constexpr auto
    name(aecs::priority_tag<3>, aecs::component::type_identity<T>) noexcept(
        noexcept(std::string_view{name(aecs::component::type_identity<T>{})}))
        -> decltype(std::string_view{name(aecs::component::type_identity<T>{})})
{
    constexpr auto n = name(aecs::component::type_identity<T>{});
    return std::string_view{n};
}

// TODO add nameof
} // namespace name_impl

inline namespace cpo
{
struct name_fn
{
    template<typename T>
    constexpr auto
    operator()(aecs::component::type_identity<T> ident) const noexcept(noexcept(
        ::aecs::component::name_impl::name(aecs::max_priority_tag, ident)))
        -> decltype(::aecs::component::name_impl::name(aecs::max_priority_tag,
                                                       ident))
    {
        return ::aecs::component::name_impl::name(aecs::max_priority_tag,
                                                  ident);
    }
};

inline constexpr auto name = name_fn{};
} // namespace cpo
} // namespace component
} // namespace aecs
