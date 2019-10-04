#pragma once

#include "aecs/component/concepts.hpp"

namespace aecs
{
namespace component
{
// a type_identity object which can be constructed for valid component types.
//
// This object will usually be instantiated in a constexpr context.
template<typename T>
struct type_identity
{
    static_assert(aecs::is_component_v<T>,
                  "T does not fulfill the component concept");

    using type = T;
};
} // namespace component
} // namespace aecs