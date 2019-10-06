#pragma once

#include "aecs/component/traits.hpp"
#include "aecs/component/type.hpp"
#include "aecs/container/polymorphic.hpp"

namespace aecs
{
template<typename T>
class wrapped_container final : public polymorphic_container
{
public:
    using container_type = aecs::component_container_t<T>;
    using value_type     = T;

private:
    container_type container_;

public:
    constexpr wrapped_container() noexcept(
        noexcept(aecs::component_type<T>::make_container()))
        : container_{aecs::component_type<T>::make_container()}
    {}

    constexpr wrapped_container(container_type&& c) noexcept(
        std::is_nothrow_move_constructible_v<container_type>)
        : container_{std::move(c)}
    {}

    constexpr wrapped_container(const container_type& c) noexcept(
        std::is_nothrow_copy_constructible_v<container_type>)
        : container_{c}
    {}

    // rule of zero holds for destructor

    constexpr container_type& get() noexcept
    {
        return container_;
    }

    constexpr const container_type& get() const noexcept
    {
        return container_;
    }

    std::size_t size() const override
    {
        return container_.size();
    }

    reference_hash operator[](std::size_t idx) override
    {
        return reference_hash{container_[idx]};
    }

    void do_push_back(const void* ptr) override
    {
        const auto& ref = *static_cast<const T*>(ptr);
        container_.push_back(ref);
    }

    std::unique_ptr<polymorphic_container> replicate() const override
    {
        return std::make_unique<wrapped_container<T>>();
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
        return aecs::component_type<T>::hash();
    }

    std::string_view component_name() const override
    {
        return aecs::component_type<T>::name();
    }
};
} // namespace aecs