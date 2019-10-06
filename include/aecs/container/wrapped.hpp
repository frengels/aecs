#pragma once

#include "aecs/component/type.hpp"
#include "aecs/container/polymorphic.hpp"

namespace aecs
{
template<typename C>
class wrapped_container final : polymorphic_container
{
public:
    using container_type = C;
    using value_type     = typename container_type::value_type;

    // static constexpr std::size_t component_hash =

private:
    container_type container_;

public:
    constexpr wrapped_container(C&& c) noexcept(
        std::is_nothrow_move_constructible_v<C>)
        : container_{std::move(c)}
    {}

    constexpr wrapped_container(const C& c) noexcept(
        std::is_nothrow_copy_constructible_v<C>)
        : container_{c}
    {}

    // rule of zero holds for destructor

    std::size_t size() const override
    {
        return container_.size();
    }

    reference_hash operator[](std::size_t idx) override
    {
        return reference_hash{container_[idx]};
    }

    std::unique_ptr<polymorphic_container> replicate() const override
    {
        return std::make_unique<polymorphic_container>(
            aecs::component_type<value_type>::make_container());
    }

    void swap_pop(std::size_t idx) override
    {
        auto it = container_.begin() + idx;

        // perform swap
        using std::swap;
        swap(*it, container_.back());

        // and pop
        container_.pop_back();
    }

    std::size_t component_hash() const override
    {
        return aecs::component_type<value_type>::hash();
    }

    std::string_view component_name() const override
    {
        return aecs::component_type<value_type>::name();
    }
}
} // namespace aecs