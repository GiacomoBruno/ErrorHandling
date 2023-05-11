#pragma once

#include <type_traits>
#include "expected_base.h"
#include "expected_type_traits.h"

namespace gb {

//monadic operations implementations
namespace detail {
template<class Exp, class F>
    requires(!std::is_void_v<expect_value_t<Exp>>)
constexpr std::decay_t<Exp> and_then_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_value_t<Exp>>)
{
    if (std::forward<Exp>(exp).has_value())
        return std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));

    return std::forward<Exp>(exp);
}

template<class Exp, class F>
    requires std::is_void_v<expect_value_t<Exp>>
constexpr std::decay_t<Exp> and_then_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F>)
{
    if (std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_same_v<std::invoke_result_t<F>, std::decay_t<Exp>>)
        {
            return std::invoke(std::forward<F>(f));
        }
        else
        {
            std::invoke(std::forward<F>(f));
            return std::forward<Exp>(exp);
        }
    }

    return std::forward<Exp>(exp);
}

template<class Exp, class F>
    requires(!std::is_void_v<expect_value_t<Exp>>)
constexpr value_transformed_t<Exp, F> transform_impl(Exp&& exp, F&& f)
{
    using result_t = value_transformed_t<Exp, F>;
    using r_value_t = expect_value_t<result_t>;

    if (std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_void_v<r_value_t>) //if T -> void then return expect
        {
            std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));
            return expect;
        }
        else // if T -> nonvoid or F -> expected<void, error> then return F call  directly
        {
            return std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));
        }
    }

    if constexpr (std::is_void_v<expect_error_t<Exp>>)
    {
        return unexpect;
    }
    else
    {
        return result_t {unexpect, std::forward<Exp>(exp).error()};
    }
}

template<class Exp, class F>
    requires std::is_void_v<expect_value_t<Exp>>
constexpr value_transformed_t<Exp, F> transform_impl(Exp&& exp, F&& f)
{
    using result_t = value_transformed_t<Exp, F>;
    using r_value_t = expect_value_t<result_t>;

    if (std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_void_v<r_value_t>) //if void -> void then return expect
        {
            std::invoke(std::forward<F>(f));
            return expect;
        }
        else // if void -> nonvoid or F -> expected<void, error> then return F call  directly
        {
            return std::invoke(std::forward<F>(f));
        }
    }

    if constexpr (std::is_void_v<expect_error_t<Exp>>)
    {
        return unexpect;
    }
    else
    {
        return result_t {unexpect, std::forward<Exp>(exp).error()};
    }
}

template<class Exp, class F>
    requires(!std::is_void_v<expect_error_t<Exp>>)
constexpr std::decay_t<Exp> or_else_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_error_t<Exp>>)
{
    if (!std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_same_v<std::invoke_result_t<F, expect_error_t<Exp>>, std::decay_t<Exp>>)
        {
            return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
        }
        else
        {
            std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
        }
    }
    return std::forward<Exp>(exp);
}

template<class Exp, class F>
    requires std::is_void_v<expect_error_t<Exp>>
constexpr std::decay_t<Exp> or_else_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F>)
{
    if (!std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_same_v<std::invoke_result_t<F>, std::decay_t<Exp>>)
        {
            return std::invoke(std::forward<F>(f));
        }
        else
        {
            std::invoke(std::forward<F>(f));
        }
    }
    return std::forward<Exp>(exp);
}

template<class Exp, class F>
    requires(!std::is_void_v<expect_error_t<Exp>>)
constexpr error_transformed_t<Exp, F> transform_error_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_error_t<Exp>>)
{
    using result_t = error_transformed_t<Exp, F>;
    using r_error_t = expect_error_t<result_t>;

    if (!std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_void_v<r_error_t>) //if E -> void then return expect
        {
            std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
            return unexpect;
        }
        else // if E -> nonvoid or F -> expected<void, error> then return F call  directly
        {
            return unexpected(std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error()));
        }
    }

    return *std::forward<Exp>(exp);
}

template<class Exp, class F>
    requires std::is_void_v<expect_error_t<Exp>>
constexpr std::decay_t<Exp> transform_error_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F>)
{
    using result_t = error_transformed_t<Exp, F>;
    using r_error_t = expect_error_t<result_t>;

    if (!std::forward<Exp>(exp).has_value())
    {
        if constexpr (std::is_void_v<r_error_t>) //if void -> void then return expect
        {
            std::invoke(std::forward<F>(f));
            return unexpect;
        }
        else // if void -> nonvoid or F -> expected<void, error> then return F call  directly
        {
            return result_t {unexpect, std::invoke(std::forward<F>(f))};
        }
    }
    return *std::forward<Exp>(exp);
}

} // namespace detail
} // namespace gb
