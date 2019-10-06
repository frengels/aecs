#include <catch2/catch.hpp>

#include "aecs/container/tag.hpp"

struct my_tag
{
    my_tag() = default; // required

    my_tag(int& i) // side effect constructor
    {
        i = 5;
    }
};

TEST_CASE("tag_container")
{
    auto tag_cont = aecs::tag_container<my_tag>();

    REQUIRE(tag_cont.size() == 0);

    tag_cont.emplace_back();

    REQUIRE(tag_cont.size() == 1);

    int i = 0;
    tag_cont.emplace_back(i);
    REQUIRE(i == 5);
    REQUIRE(tag_cont.size() == 2);

    auto it = tag_cont.begin();

    auto end = tag_cont.end();

    REQUIRE(it != end);
    REQUIRE((it + 2) == end);

    auto t = *it;
    ++it;
    ++it;

    REQUIRE(it == end);
}