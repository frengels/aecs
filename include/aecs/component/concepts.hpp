#pragma once

#include <type_traits>

namespace aecs
{
template<typename T, typename = void>
struct is_component : std::false_type
{};

// A component must be trivially copyable. This means there is no special
// behaviour in the copy/move/destructor.
template<typename T>
struct is_component<T, std::enable_if_t<std::is_trivially_copyable_v<T>>>
    : std::true_type
{};

template<typename T>
inline constexpr bool is_component_v = is_component<T>::value;

template<typename T, typename = void>
struct is_tag_component : std::false_type
{};

template<typename T>
struct is_tag_component<
    T,
    std::enable_if_t<std::is_empty_v<T> &&
                     std::is_trivially_default_constructible_v<T>>>
    : is_component<T>
{};

template<typename T>
inline constexpr bool is_tag_component_v = is_tag_component<T>::value;
} // namespace aecs