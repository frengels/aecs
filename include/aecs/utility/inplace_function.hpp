#pragma once

#include <type_traits>

namespace aecs
{
namespace detail
{
template<std::size_t Cap>
union aligned_storage_helper
{
    struct double1
    {
        double a;
    };

    struct double4
    {
        double a[4];
    };

    template<typename T>
    using maybe = std::conditional_t<(Cap >= sizeof(T)), T, char>;

    char               real_data[Cap];
    maybe<int>         a;
    maybe<long>        b;
    maybe<long long>   c;
    maybe<void*>       d;
    maybe<double1>     f;
    maybe<double4>     g;
    maybe<long double> h;
};

template<std::size_t Cap,
         std::size_t Align = alignof(aligned_storage_helper<Cap>)>
struct aligned_storage
{
    using type = std::aligned_storage_t<Cap, Align>;
};

template<std::size_t Cap,
         std::size_t Align = alignof(aligned_storage_helper<Cap>)>
using aligned_storage_t = typename aligned_storage<Cap, Align>::type;
} // namespace detail

template<typename Signature,
         std::size_t Capacity = sizeof(void*),
         std::size_t Align    = alignof(detail::aligned_storage_t<Capacity>)>
class inplace_function;

namespace detail
{
template<typename T>
struct is_inplace_function : std::false_type
{};

template<typename Signature, std::size_t Capacity, std::size_t Align>
struct is_inplace_function<aecs::inplace_function<Signature, Capacity, Align>>
    : std::true_type
{};

template<typename T>
constexpr bool is_inplace_function_v = is_inplace_function<T>::value;
} // namespace detail

// a trivial inplace_function, which only takes trivially_copyable callable
// types. Mostly intended to be used with lambdas. Because of being trivial,
// optimization possibilities are much better than std::function.
template<typename Ret,
         typename... Args,
         std::size_t Capacity,
         std::size_t Align>
class inplace_function<Ret(Args...), Capacity, Align>
{
private:
    template<typename, std::size_t, std::size_t>
    friend class inplace_function;

private:
    std::aligned_storage_t<Capacity, Align> callable_;
    std::add_pointer_t<Ret(void*, Args...)> helper_fn_{nullptr};

public:
    inplace_function() = default;

    template<
        typename Callable,
        typename = std::enable_if_t<std::is_trivially_copyable_v<Callable> &&
                                    (sizeof(Callable) <= Capacity) &&
                                    std::is_invocable_v<Callable&, Args...> &&
                                    !detail::is_inplace_function_v<Callable>>>
    constexpr inplace_function(const Callable& c) noexcept(
        std::is_nothrow_copy_constructible_v<Callable>)
        : helper_fn_{[](void* callable, Args... args) -> Ret {
              Callable& c =
                  *std::launder(reinterpret_cast<Callable*>(callable));
              return static_cast<Ret>(
                  std::invoke(c, std::forward<Args>(args)...));
          }}
    {
        new (&callable_) Callable(c);
    }

    // no need for move, since trivially copyable

    // construct from other inplace_function
    template<std::size_t UCap,
             std::size_t UAlign,
             typename = std::enable_if_t<
                 ((Capacity != UCap) || (Align != UAlign)) &&
                 (sizeof(std::aligned_storage_t<UCap, UAlign>) <=
                  sizeof(std::aligned_storage_t<Capacity, Align>))>>
    constexpr inplace_function(
        const inplace_function<Ret(Args...), UCap, UAlign>& other) noexcept
        : callable_{reinterpret_cast<const decltype(callable_)&>(
              other.callable_)},
          helper_fn_{other.helper_fn_}
    {}

    inplace_function(const inplace_function&) = default;

    explicit constexpr operator bool() const noexcept
    {
        return helper_fn_ != nullptr;
    }

    Ret operator()(Args... args) const
    {
        return helper_fn_(
            const_cast<void*>(static_cast<const void*>(&callable_)),
            std::forward<Args>(args)...);
    }

    void swap(inplace_function& other) noexcept
    {
        using std::swap;
        swap(callable_, other.callable_);
        swap(helper_fn_, other.helper_fn_);
    }
};
} // namespace aecs