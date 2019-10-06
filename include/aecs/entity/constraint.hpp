#pragma once

#include <array>

#include "aecs/component/hash.hpp"

namespace aecs
{
namespace entity
{
enum struct access
{
    exclude,
    write,
    read
};

template<access A, typename T>
struct static_constraint
{
    using component_type               = T;
    static constexpr auto access_value = A;
};

class constraint
{
private:
    // None of these are allowed to change after initialization. Important
    // assumptions are made based on this.
    std::size_t          component_hash_;
    aecs::entity::access access_;

public:
    template<access A, typename T>
    constexpr constraint(static_constraint<A, T>) noexcept
        : constraint{A, aecs::component::type_identity<T>{}}
    {}

    template<typename T>
    constexpr constraint(access a, std::in_place_type_t<T>) noexcept
        : constraint{a, aecs::component::type_identity<T>{}}
    {}

    template<typename T>
    constexpr constraint(access a, aecs::component::type_identity<T>) noexcept
        : component_hash_{[]() {
              // make sure it's really computed at compilation time
              constexpr auto h =
                  aecs::component::hash(aecs::component::type_identity<T>{});
              return h;
          }()},
          access_{a}
    {}

    constexpr auto hash() const noexcept
    {
        return component_hash_;
    }

    constexpr auto access() const noexcept
    {
        return access_;
    }

    constexpr bool operator==(const constraint& other) const noexcept
    {
        return hash() == other.hash() && this->access() == other.access();
    }

    constexpr bool operator!=(const constraint& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const constraint& other) const noexcept
    {
        switch (this->access())
        {
        case access::exclude:
            switch (other.access())
            {
            case access::exclude:
                return hash() < other.hash();
            case access::write:
            case access::read:
                return true;
            }
        case access::write:
            switch (other.access())
            {
            case access::exclude:
                return false;
            case access::write:
                return hash() < other.hash();
            case access::read:
                return true;
            }
        case access::read:
            switch (other.access())
            {
            case access::exclude:
            case access::write:
                return false;
            case access::read:
                return hash() < other.hash();
            }
        }

        // never gets here
        return false;
    }

    constexpr bool allow_parallelism(const constraint& other) const noexcept
    {
        if (hash() != other.hash())
        {
            // not the same hash, so different component.
            return true;
        }

        // beyond this point all have the same hash

        if (access() == access::exclude || other.access() == access::exclude)
        {
            return true;
        }

        if (access() == access::read && other.access() == access::read)
        {
            return true;
        }

        return false;
    }
};

template<std::size_t N>
class constraint_list
{
private:
    const std::array<constraint, N> arr_;

public:
    template<typename... Ts>
    constexpr constraint_list(Ts&&... constraints) noexcept(
        (std::is_nothrow_constructible_v<constraint, Ts> && ...))
        : arr_{[&]() {
              auto mutable_arr = std::array<constraint, sizeof...(Ts)>{
                  constraint{std::forward<Ts>(constraints)}...};
              std::sort(mutable_arr.begin(), mutable_arr.end());
              return mutable_arr;
          }()}
    {}

    static constexpr std::size_t size() noexcept
    {
        return N;
    }

    constexpr const constraint* data() const noexcept
    {
        return arr_.data();
    }

    constexpr auto begin() const noexcept
    {
        return arr_.begin();
    }

    constexpr auto end() const noexcept
    {
        return arr_.end();
    }

    constexpr const constraint& operator[](std::size_t idx) const noexcept
    {
        return arr_[idx];
    }
};

template<typename... Ts>
constraint_list(Ts&&...)->constraint_list<sizeof...(Ts)>;

class constraint_view
{
private:
    const constraint* first_;
    const constraint* last_;

public:
    template<std::size_t N>
    constexpr constraint_view(const constraint_list<N>& cs) noexcept
        : first_{cs.data()}, last_{first_ + N}
    {}

    constexpr const auto* begin() const noexcept
    {
        return first_;
    }

    constexpr const auto* end() const noexcept
    {
        return last_;
    }

    constexpr std::size_t size() const noexcept
    {
        return last_ - first_;
    }

    constexpr bool allow_parallelism(constraint_view other) const noexcept
    {
        for (const auto& c1 : *this)
        {
            for (const auto& c2 : other)
            {
                if (c1.access() == access::exclude ||
                    c2.access() == access::exclude)
                {
                    // if any exclude has the same hash it makes the entire
                    // constraint parallel
                    if (c1.hash() == c2.hash() && c1.access() != c2.access())
                    {
                        return true;
                    }
                }
                else if (!c1.allow_parallelism(c2))
                {
                    // I can only do this here because the constraint are
                    // guaranteed to be sorted with exclude constraints coming
                    // first as they're the most impactful.
                    return false;
                }
            }
        }

        return true;
    }

    constexpr const constraint& operator[](std::size_t idx) const noexcept
    {
        return first_[idx];
    }
};
} // namespace entity
} // namespace aecs