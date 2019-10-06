#pragma once

#include "aecs/component/type.hpp"

namespace aecs
{
template<typename T>
using component_container_t =
    decltype(aecs::component_type<T>::make_container());
}