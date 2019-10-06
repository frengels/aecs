#include <catch2/catch.hpp>

#include "aecs/component/type.hpp"

struct char_named
{
    static constexpr auto name = "char_named";
};

struct charstar_named
{
    static constexpr const char* name = "charstar_named";
};

struct strview_named
{
    static constexpr std::string_view name = "strview_named";
};

namespace my
{
struct external_named
{};

constexpr auto component_name(aecs::component_type<my::external_named>)
{
    return "external_named";
}
} // namespace my

struct unspecified_name
{};

TEST_CASE("nameof")
{
    constexpr auto char_n =
        aecs::component_name(aecs::component_type<char_named>{});
    static_assert(char_n == "char_named");
    constexpr auto charstar_n =
        aecs::component_name(aecs::component_type<charstar_named>{});
    static_assert(charstar_n == "charstar_named");
    constexpr auto strview_n =
        aecs::component_name(aecs::component_type<strview_named>{});
    static_assert(strview_n == "strview_named");
    constexpr auto external_n =
        aecs::component_name(aecs::component_type<my::external_named>{});
    static_assert(external_n == "external_named");

    constexpr auto unspecified_n =
        aecs::component_name(aecs::component_type<unspecified_name>{});
    static_assert(unspecified_n == "unspecified_name");

    static_assert(unspecified_n ==
                  aecs::component_type<unspecified_name>{}.name());
}