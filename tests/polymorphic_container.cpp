#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "aecs/container/wrapped.hpp"

TEST_CASE("polymorphic_container")
{
    SECTION("polymorphic")
    {
        std::unique_ptr<aecs::polymorphic_container> cont =
            std::make_unique<aecs::wrapped_container<int>>();

        REQUIRE(cont->has_component<int>());
        REQUIRE(!cont->has_component<float>());

        BENCHMARK("poly_push_back")
        {
            for (auto i = 0; i < 10; ++i)
            {
                cont->push_back<int>(i);
            }
        };
    }

    SECTION("concrete")
    {
        auto ivec = std::vector<int>{};

        BENCHMARK("concrete_push_back")
        {
            for (auto i = 0; i < 10; ++i)
            {
                ivec.push_back(i);
            }
        };
    }
}