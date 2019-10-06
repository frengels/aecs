#pragma once

#include <type_traits>
#include <utility>

namespace aecs
{

template<typename T>
class tag_container : private T
{
    static_assert(std::is_same_v<T, std::remove_cv_t<T>>,
                  "type cannot be const/volatile qualified");
    static_assert(std::is_object_v<T>);
    static_assert(std::is_empty_v<T>, "Tags must be empty");
    static_assert(std::is_trivially_default_constructible_v<T>,
                  "We must be able to get a default tag to return");
    static_assert(std::is_trivially_copyable_v<T>);

public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    class iterator;

private:
    std::size_t size_{};

public:
    tag_container() = default;

    constexpr iterator begin() const noexcept
    {
        return iterator{0};
    }

    constexpr iterator end() const noexcept
    {
        return iterator{size_};
    }

    constexpr std::size_t size() const noexcept
    {
        return size_;
    }

    template<typename... Args>
    constexpr void emplace_back(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        auto side_effects = T(std::forward<Args>(args)...);
        ++size_; // increment after for exceptions
        // trivially_copyable -> no side effects from destructing, only
        // constructing (possibly).
    }

    constexpr void
    push_back(T&& t) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        emplace_back(std::move(t));
    }

    constexpr void
    push_back(const T& t) noexcept(std::is_nothrow_copy_constructible_v<T>)
    {
        emplace_back(t);
    }

    constexpr reference operator[](std::size_t idx) const noexcept
    {
        assert(idx < size());
        return *this;
    }
};

template<typename T>
class tag_container<T>::iterator : private T
{
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    using difference_type = std::ptrdiff_t;

private:
    difference_type index_;

public:
    constexpr iterator(std::size_t idx) noexcept
        : T{}, index_{static_cast<difference_type>(idx)}
    {}

    constexpr bool operator==(iterator other) const noexcept
    {
        return index_ == other.index_;
    }

    constexpr bool operator!=(iterator other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(iterator other) const noexcept
    {
        return index_ < other.index_;
    }

    constexpr bool operator>(iterator other) const noexcept
    {
        return index_ > other.index_;
    }

    constexpr bool operator<=(iterator other) const noexcept
    {
        return index_ <= other.index_;
    }

    constexpr bool operator>=(iterator other) const noexcept
    {
        return index_ >= other.index_;
    }

    constexpr iterator& operator++() noexcept
    {
        ++index_;
        return *this;
    }

    constexpr iterator& operator--() noexcept
    {
        --index_;
        return *this;
    }

    constexpr iterator operator++(int) noexcept
    {
        auto res = *this;
        ++(*this);
        return res;
    }

    constexpr iterator operator--(int) noexcept
    {
        auto res = *this;
        --(*this);
        return res;
    }

    constexpr iterator& operator+=(difference_type diff) noexcept
    {
        index_ += diff;
        return *this;
    }

    constexpr iterator& operator-=(difference_type diff) noexcept
    {
        index_ -= diff;
        return *this;
    }

    constexpr difference_type operator-(iterator other) const noexcept
    {
        return index_ - other.index_;
    }

    constexpr iterator operator-(difference_type diff) const noexcept
    {
        auto res = *this;
        res -= diff;
        return res;
    }

    constexpr iterator operator+(difference_type diff) const noexcept
    {
        auto res = *this;
        res += diff;
        return res;
    }

    friend constexpr iterator operator+(difference_type diff,
                                        iterator        it) noexcept
    {
        return it + diff;
    }

    constexpr reference operator*() noexcept
    {
        return *this;
    }

    constexpr pointer operator->() noexcept
    {
        return this;
    }
};
} // namespace aecs