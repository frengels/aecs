#pragma once

#include "aecs/component/hash.hpp"

namespace aecs
{
namespace system
{
enum struct access
{
    read,
    write,
    exclude
};

template<access A, typename T>
struct static_constraint
{
    using component_type = T;
    using access_type    = A;
};

class constraint
{
private:
    // None of these are allowed to change after initialization. Important
    // assumptions are made based on this.
    const std::size_t          component_hash_;
    const aecs::system::access access_;

public:
    template<access A, typename T>
    constexpr constraint(static_constraint<A, T>) noexcept
        : component_hash_{[]() {
              // make sure the hash is obtained at compile time
              constexpr auto h =
                  aecs::component::hash(aecs::component::type_identity<T>{});
              return h;
          }()},
          access_{A}
    {}

    constexpr auto hash() const noexcept
    {
        return component_hash_;
    }

    constexpr auto access() const noexcept
    {
        return access_;
    }

    constexpr bool allow_concurrent(const constraint& other) const noexcept
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
} // namespace system
} // namespace aecs