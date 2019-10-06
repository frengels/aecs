#pragma once

#include <memory>

namespace aecs
{
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
    virtual ~polymorphic_container() = 0;

    virtual std::size_t size() const = 0;

    virtual reference_hash operator[](std::size_t idx) = 0;

    // return a new container of the same internal type.
    virtual std::unique_ptr<polymorphic_container> replicate() const = 0;

    virtual swap_pop(std::size_t index) = 0;

    virtual std::size_t component_hash() const = 0;

    virtual std::string_view component_name() const = 0;

    template<typename T>
    bool has_component() const
    {
        return aecs::component_type<T>::hash() == component_hash();
    }
};
} // namespace aecs