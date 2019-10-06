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
    static constexpr auto             make_container();
};

namespace detail
{
struct component_name_fn
{
private:
    template<typename T>
    static constexpr auto
        impl(aecs::priority_tag<4>,
             aecs::component_type<T>) noexcept(noexcept(T::name))
            -> decltype(T::name)
    {
        return T::name;
    }

    template<typename T>
    static constexpr auto
        impl(aecs::priority_tag<3>, aecs::component_type<T>) noexcept(
            noexcept(name(aecs::component_type<T>{})))
            -> decltype(name(aecs::component_type<T>{}))
    {
        return name(aecs::component_type<T>{});
    }

    template<typename T>
    static constexpr auto
        impl(aecs::priority_tag<0>,
             aecs::component_type<T>) noexcept(noexcept(aecs::nameof_type<T>()))
            -> decltype(aecs::nameof_type<T>())
    {
        return aecs::nameof_type<T>();
    }

public:
    template<typename T>
    constexpr auto operator()(aecs::component_type<T>) const
        noexcept(noexcept(impl(aecs::max_priority_tag,
                               aecs::component_type<T>{})))
            -> decltype(impl(aecs::max_priority_tag, aecs::component_type<T>{}),
                        std::string_view{})
    {
        constexpr auto n =
            impl(aecs::max_priority_tag, aecs::component_type<T>{});
        // make sure string_view conversion is constexpr
        constexpr auto strv_n = std::string_view{n};
        return strv_n;
    }
};

inline constexpr auto component_name = component_name_fn{};
} // namespace detail

struct component_hash_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component_type<T>) const noexcept
    {
        constexpr auto n    = aecs::component_type<T>::name();
        constexpr auto hash = aecs::sha1(n);
        return hash;
    }
};

namespace make_container_impl
{
template<typename T>
constexpr auto
    make_container(aecs::priority_tag<4>,
                   component_type<T>) noexcept(noexcept(T::make_container()))
        -> decltype(T::make_container())
{
    return T::make_container();
}

template<typename T>
constexpr auto make_container(
    aecs::priority_tag<3>,
    component_type<T>) noexcept(noexcept(typename T::container_type{}))
    -> decltype(typename T::container_type{})
{
    return typename T::container_type{};
}

template<typename T>
constexpr auto
    make_container(aecs::priority_tag<2>, component_type<T>) noexcept(
        noexcept(make_container(aecs::component_type<T>{})))
        -> decltype(make_container(aecs::component_type<T>{}))
{
    return make_container(aecs::component_type<T>{});
}

// default container is std::vector
template<typename T>
constexpr std::vector<T> make_container(
    aecs::priority_tag<0>,
    component_type<T>) noexcept(std::is_nothrow_constructible_v<std::vector<T>>)
{
    return {};
}
} // namespace make_container_impl

inline namespace cpo
{
struct make_container_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component_type<T> ct) const noexcept(
        noexcept(::aecs::make_container_impl::make_container(max_priority_tag,
                                                             ct)))
        -> decltype(
            ::aecs::make_container_impl::make_container(max_priority_tag, ct))
    {
        return ::aecs::make_container_impl::make_container(max_priority_tag,
                                                           ct);
    }
};

inline constexpr auto make_container = make_container_fn{};
} // namespace cpo

inline constexpr auto component_hash = component_hash_fn{};

template<typename T>
constexpr std::string_view component_type<T>::name() noexcept
{
    return aecs::detail::component_name(component_type<T>{});
}

template<typename T>
constexpr std::size_t component_type<T>::hash() noexcept
{
    return aecs::component_hash(component_type<T>{});
}

template<typename T>
constexpr auto component_type<T>::make_container()
{
    return aecs::make_container(component_type<T>{});
}
} // namespace aecs