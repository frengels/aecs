#pragma once

#include "aecs/component/name.hpp"
#include "aecs/component/type_identity.hpp"
#include "aecs/utility/sha1.hpp"

namespace aecs
{
namespace component
{
struct hash_fn
{
    template<typename T>
    constexpr auto operator()(aecs::component::type_identity<T>) const noexcept
    {
        constexpr auto n =
            aecs::component::name(aecs::component::type_identity<T>{});
        constexpr auto hash = aecs::sha1(n);
        return hash;
    }
};

inline constexpr auto hash = hash_fn{};
} // namespace component
} // namespace aecs