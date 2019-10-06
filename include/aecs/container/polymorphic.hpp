#pragma once

#include <memory>

#include "aecs/component/traits.hpp"
#include "aecs/component/type.hpp"

namespace aecs
{
template<typename T>
class wrapped_container;

class reference_hash
{
private:
    void*       ptr_;
    std::size_t hash_;

public:
    template<typename T>
    constexpr reference_hash(T& ref) noexcept
        : ptr_{std::addressof(ref)}, hash_{aecs::component_type<T>::hash()}
    {}

    template<typename T>
    constexpr T& get() const noexcept
    {
        assert(aecs::component_type<T>::hash() == hash_ &&
               "hash doesn't match, this value is not of type T");
        return *static_cast<T*>(ptr_);
    }

    constexpr std::size_t hash() const noexcept
    {
        return hash_;
    }
};

class polymorphic_container
{
public:
    virtual ~polymorphic_container() = default;

    virtual std::size_t size() const = 0;

    virtual reference_hash operator[](std::size_t idx) = 0;

private:
    virtual void do_push_back(const void* ptr) = 0;

public:
    // return a new container of the same internal type.
    virtual std::unique_ptr<polymorphic_container> replicate() const = 0;

    virtual void swap_pop(std::size_t index) = 0;

    virtual std::size_t component_hash() const = 0;

    virtual std::string_view component_name() const = 0;

    template<typename T>
    bool has_component() const
    {
        return aecs::component_type<T>::hash() == component_hash();
    }

    template<typename T, typename... Args>
    void push_back(Args&&... args)
    {
        assert(has_component<T>() && "This is the wrong component type");
        auto val = T(std::forward<Args>(args)...);
        do_push_back(std::addressof(val));
    }

    template<typename T>
    aecs::component_container_t<T>& get() noexcept
    {
        assert(has_component<T>() && "This is the wrong component type");
        return static_cast<wrapped_container<T>&>(*this).get();
    }

    template<typename T>
    const aecs::component_container_t<T>& get() const noexcept
    {
        assert(has_component<T>() && "This is the wrong component type");
        return static_cast<const wrapped_container<T>&>(*this).get();
    }
};
} // namespace aecs