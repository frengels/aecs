#include <catch2/catch.hpp>

#include "aecs/component/nameof.hpp"

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
} // namespace my

namespace aecs
{
namespace component
{
template<>
struct nameof<my::external_named>
{
    static constexpr std::string_view value = "external_named";
};
} // namespace component
} // namespace aecs

TEST_CASE("nameof")
{
    constexpr auto char_n     = aecs::component::nameof_v<char_named>;
    constexpr auto charstar_n = aecs::component::nameof_v<charstar_named>;
    constexpr auto strview_n  = aecs::component::nameof_v<strview_named>;
    constexpr auto external_n = aecs::component::nameof_v<my::external_named>;
}