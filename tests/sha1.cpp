#include <catch2/catch.hpp>

#include <iostream>

#include "aecs/component/type.hpp"
#include "aecs/utility/sha1.hpp"

struct ex1
{
    static constexpr auto name = "example_component_name";
};

TEST_CASE("sha1")
{
    constexpr auto str = std::string_view{"hello"};

    constexpr auto hash_str = aecs::sha1(str);

    static_assert(hash_str == 0xaaf4c61ddcc5e8a2);

    constexpr auto example_hash = aecs::sha1("example_component_name");

    static_assert(example_hash == 0x40973e3a544d02bc);

    constexpr auto hash = aecs::component_type<ex1>{}.hash();

    static_assert(hash == example_hash);
}