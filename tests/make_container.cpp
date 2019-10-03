#include <catch2/catch.hpp>

#include "aecs/component/make_container.hpp"

#include <deque>
#include <list>

struct nonempty_component {
  int i, j, k;
};

struct custom_container_type {
  using container_type = std::list<custom_container_type>;
};

struct custom_fn_container_type {
  static auto make_container() noexcept(
      std::is_nothrow_constructible_v<std::deque<custom_fn_container_type>>) {
    return std::deque<custom_fn_container_type>{};
  }
};

TEST_CASE("make_container") {
  // creates vector
  auto nonempty_cont = aecs::make_container<nonempty_component>();
  static_assert(
      std::is_same_v<std::vector<nonempty_component>, decltype(nonempty_cont)>);

  // specified required type
  auto custom_cont = aecs::make_container<custom_container_type>();
  static_assert(
      std::is_same_v<std::list<custom_container_type>, decltype(custom_cont)>);

  // custom function to create container
  auto custom_fn_cont = aecs::make_container<custom_fn_container_type>();
  static_assert(std::is_same_v<std::deque<custom_fn_container_type>,
                               decltype(custom_fn_cont)>);
}