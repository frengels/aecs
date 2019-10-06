#include <catch2/catch.hpp>

#include "aecs/entity/constraint.hpp"

TEST_CASE("constraint")
{
    using aecs::entity::access;
    using aecs::entity::constraint;
    using aecs::entity::constraint_list;
    using aecs::entity::constraint_view;

    auto c_arr1 =
        constraint_list{constraint{access::read, std::in_place_type<int>},
                        constraint{access::write, std::in_place_type<float>}};

    auto c_arr2 =
        constraint_list{constraint{access::read, std::in_place_type<int>},
                        constraint{access::write, std::in_place_type<char>}};

    auto c_arr3 =
        constraint_list{constraint{access::exclude, std::in_place_type<char>},
                        constraint{access::write, std::in_place_type<int>}};

    // cannot be constructed as a constexpr
    auto cs1 = constraint_view{c_arr1};
    auto cs2 = constraint_view{c_arr2};
    auto cs3 = constraint_view{c_arr3};

    REQUIRE(cs1.allow_parallelism(cs2));

    REQUIRE(cs2.allow_parallelism(cs3));

    REQUIRE(!cs1.allow_parallelism(cs1));
    REQUIRE(!cs2.allow_parallelism(cs2));
    REQUIRE(!cs3.allow_parallelism(cs3));
}