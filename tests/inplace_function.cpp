#include <catch2/catch.hpp>

#include "aecs/utility/inplace_function.hpp"

static int k = 0;

void g()
{
    k = 10;
}

TEST_CASE("inplace_function")
{
    aecs::inplace_function<void()> f{};

    // ignores the non void return type
    f = [i = 4]() { return i; };
    f();

    // correctly capture environment
    int i = 0;
    f     = [&]() { i = 5; };
    REQUIRE(i == 0);
    f();
    REQUIRE(i == 5);

    // this lambda does not fit in 8 bytes, it's 9 bytes
    auto l = [i = 0, j = 5, k = 'a']() {};
    static_assert(!std::is_assignable_v<decltype(f), decltype(l)>);

    f = &g;
    f();
    REQUIRE(k == 10);

    auto f1 = aecs::inplace_function<void()>{};

    REQUIRE(f);
    REQUIRE(!f1);

    f.swap(f1);

    REQUIRE(f1);
    REQUIRE(!f);

    f = f1;
    REQUIRE(f1);
    REQUIRE(f);

    auto f2 = aecs::inplace_function<void(), 16>{};
    REQUIRE(!f2);

    f2 = f1;
    REQUIRE(f2);
    k = 0;
    REQUIRE(k == 0);
    f2();
    REQUIRE(k == 10);

    // none of this can be constexpr sadly, because of the aligned_storage I
    // presume.
}