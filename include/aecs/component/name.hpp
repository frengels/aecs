#pragma once

#include <string_view>

#include "aecs/component/type_identity.hpp"
#include "aecs/utility/nameof_type.hpp"
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
         aecs::component::type_identity<T>) noexcept(noexcept(T::name))
        -> decltype(T::name)
{
    return T::name;
}

template<typename T>
constexpr auto
    name(aecs::priority_tag<3>, aecs::component::type_identity<T>) noexcept(
        noexcept(name(aecs::component::type_identity<T>{})))
        -> decltype(name(aecs::component::type_identity<T>{}))
{
    return name(aecs::component::type_identity<T>{});
}

template<typename T>
constexpr auto
    name(aecs::priority_tag<2>, aecs::component::type_identity<T>) noexcept(
        noexcept(aecs::nameof_type<T>())) -> decltype(aecs::nameof_type<T>())
{
    return aecs::nameof_type<T>();
}
} // namespace name_impl

inline namespace cpo
{
struct name_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component::type_identity<T>) const
        noexcept(noexcept(::aecs::component::name_impl::name(
            aecs::max_priority_tag,
            aecs::component::type_identity<T>{})))
            -> decltype(::aecs::component::name_impl::name(
                            aecs::max_priority_tag,
                            aecs::component::type_identity<T>{}),
                        std::string_view{})
    {
        constexpr auto n = ::aecs::component::name_impl::name(
            aecs::max_priority_tag, aecs::component::type_identity<T>{});
        return n;
    }
};

inline constexpr auto name = name_fn{};
} // namespace cpo
} // namespace component
} // namespace aecs
