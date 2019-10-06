#pragma once

#include "aecs/component/concepts.hpp"
#include "aecs/utility/nameof_type.hpp"
#include "aecs/utility/priority_tag.hpp"
#include "aecs/utility/sha1.hpp"

namespace aecs
{
// a type_identity object which can be constructed for valid component types.
//
// This object will usually be instantiated in a constexpr context.
template<typename T>
struct component_type
{
    static_assert(aecs::is_component_v<T>,
                  "T does not fulfill the component concept");

    using type = T;

    static constexpr std::string_view name() noexcept;
    static constexpr std::size_t      hash() noexcept;
};

namespace component_name_impl
{
template<typename T>
constexpr auto
    component_name(aecs::priority_tag<4>,
                   aecs::component_type<T>) noexcept(noexcept(T::name))
        -> decltype(T::name)
{
    return T::name;
}

template<typename T>
constexpr auto
    component_name(aecs::priority_tag<3>, aecs::component_type<T>) noexcept(
        noexcept(component_name(aecs::component_type<T>{})))
        -> decltype(component_name(aecs::component_type<T>{}))
{
    return component_name(aecs::component_type<T>{});
}

template<typename T>
constexpr auto component_name(
    aecs::priority_tag<2>,
    aecs::component_type<T>) noexcept(noexcept(aecs::nameof_type<T>()))
    -> decltype(aecs::nameof_type<T>())
{
    return aecs::nameof_type<T>();
}
} // namespace component_name_impl

inline namespace cpo
{
struct component_name_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component_type<T>) const noexcept(noexcept(
        ::aecs::component_name_impl::component_name(aecs::max_priority_tag,
                                                    aecs::component_type<T>{})))
        -> decltype(::aecs::component_name_impl::component_name(
                        aecs::max_priority_tag,
                        aecs::component_type<T>{}),
                    std::string_view{})
    {
        constexpr auto n = ::aecs::component_name_impl::component_name(
            aecs::max_priority_tag, aecs::component_type<T>{});
        // make sure string_view conversion is constexpr
        constexpr auto strv_n = std::string_view{n};
        return strv_n;
    }
};

inline constexpr auto component_name = component_name_fn{};
} // namespace cpo

struct component_hash_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component_type<T>) const noexcept
    {
        constexpr auto n    = aecs::component_name(aecs::component_type<T>{});
        constexpr auto hash = aecs::sha1(n);
        return hash;
    }
};

inline constexpr auto component_hash = component_hash_fn{};

template<typename T>
constexpr std::string_view component_type<T>::name() noexcept
{
    return aecs::component_name(component_type<T>{});
}

template<typename T>
constexpr std::size_t component_type<T>::hash() noexcept
{
    return aecs::component_hash(component_type<T>{});
}
} // namespace aecs