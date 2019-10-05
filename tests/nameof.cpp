#include <catch2/catch.hpp>

#include "aecs/utility/nameof_type.hpp"

namespace abc
{
namespace def
{
template<typename T, typename U>
struct hi
{};
} // namespace def
} // namespace abc

TEST_CASE("nameof")
{
    constexpr auto i = aecs::nameof_type<int>();
    static_assert(i == "int");

    using abc::def::hi;

    constexpr auto complex_type =
        aecs::nameof_type<volatile const hi<std::integral_constant<int, 5>,
                                            long long unsigned int>&&>();

    constexpr auto expected =
        "abc::def::hi<std::integral_constant<int, 5>, long long unsigned int>";

    static_assert(complex_type == expected);
}