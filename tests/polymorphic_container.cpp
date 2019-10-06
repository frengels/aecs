#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "aecs/container/wrapped.hpp"

struct my_tag
{};

TEST_CASE("polymorphic_container")
{
    SECTION("polymorphic")
    {
        std::unique_ptr<aecs::polymorphic_container> cont =
            std::make_unique<aecs::wrapped_container<int>>();

        std::unique_ptr<aecs::polymorphic_container> tag_cont =
            std::make_unique<aecs::wrapped_container<my_tag>>();

        REQUIRE(cont->has_component<int>());
        REQUIRE(!cont->has_component<float>());
        REQUIRE(!cont->has_component<int*>());

        auto& real_cont = cont->get<int>();
        REQUIRE(real_cont.size() == 0);

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