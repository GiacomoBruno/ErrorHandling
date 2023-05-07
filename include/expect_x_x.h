#pragma once

#include "expect_base.h"
#include <memory>
template <class T, class E>
    requires(!std::is_void_v<T>) && (!std::is_void_v<E>)
struct expect<T, E>
{

    #pragma region constructors

    #pragma region default empty/copy/move constructors

    constexpr expect() noexcept(std::is_nothrow_default_constructible_v<T>) // strengthened
        requires std::is_default_constructible_v<T>
        : m_has_value(true)
    {
        std::construct_at(std::addressof(m_value_error.m_value));
    }

    constexpr expect(empty_error_t) noexcept(std::is_nothrow_default_constructible_v<E>) // strengthened
        requires std::is_default_constructible_v<E>
        : m_has_value(false)
    {
        std::construct_at(std::addressof(m_value_error.m_error));
    }

    constexpr expect(const expect &) = delete;

    constexpr expect(const expect &)
        requires(std::is_copy_constructible_v<T> &&
                 std::is_copy_constructible_v<E> &&
                 std::is_trivially_copy_constructible_v<T> &&
                 std::is_trivially_copy_constructible_v<E>)
    = default;

    constexpr expect(const expect &other) noexcept(std::is_nothrow_copy_constructible_v<T> &&std::is_nothrow_copy_constructible_v<E>) // strengthened
        requires(std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E> &&
                 !(std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<E>))
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            std::construct_at(std::addressof(m_value_error.m_value), other.m_value_error.m_value);
        }
        else
        {
            std::construct_at(std::addressof(m_value_error.m_error), other.m_value_error.m_error);
        }
    }

    constexpr expect(expect &&)
        requires(std::is_move_constructible_v<T> && std::is_move_constructible_v<E> && std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>)
    = default;

    constexpr expect(expect &&other) noexcept(std::is_nothrow_move_constructible_v<T> &&std::is_nothrow_move_constructible_v<E>)
        requires(std::is_move_constructible_v<T> && std::is_move_constructible_v<E> &&
                 !(std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>))
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            std::construct_at(std::addressof(m_value_error.m_value), std::move(other.m_value_error.m_value));
        }
        else
        {
            std::construct_at(std::addressof(m_value_error.m_error), std::move(other.m_value_error.m_error));
        }
    }

    #pragma endregion

    #pragma region conversion constructors

    template <class _Up, class _OtherErr>
        requires detail::can_convert<T, E, _Up, _OtherErr, const _Up &, const _OtherErr &>::value
    constexpr explicit(!std::is_convertible_v<const _Up &, T> || !std::is_convertible_v<const _OtherErr &, E>)
        expect(const expect<_Up, _OtherErr> &other) noexcept(std::is_nothrow_constructible_v<T, const _Up &> &&
                                                                 std::is_nothrow_constructible_v<E, const _OtherErr &>) // strengthened
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            std::construct_at(std::addressof(m_value_error.m_value), other.m_value_error.m_value);
        }
        else
        {
            std::construct_at(std::addressof(m_value_error.m_error), other.m_value_error.m_error);
        }
    }

    template <class _Up, class _OtherErr>
        requires detail::can_convert<T, E, _Up, _OtherErr, _Up, _OtherErr>::value
    constexpr explicit(!std::is_convertible_v<_Up, T> || !std::is_convertible_v<_OtherErr, E>)
        expect(expect<_Up, _OtherErr> &&other) noexcept(std::is_nothrow_constructible_v<T, _Up> &&std::is_nothrow_constructible_v<E, _OtherErr>) // strengthened
        : m_has_value(other.m_has_value)
    {
        if (m_has_value)
        {
            std::construct_at(std::addressof(m_value_error.m_value), std::move(other.m_value_error.m_value));
        }
        else
        {
            std::construct_at(std::addressof(m_value_error.m_error), std::move(other.m_value_error.m_error));
        }
    }

    #pragma endregion

    template <class _Up = T>
        requires(!std::is_same_v<std::remove_cvref_t<_Up>, std::in_place_t> && !std::is_same_v<expect, std::remove_cvref_t<_Up>> &&
                 !is_unexpect_v<std::remove_cvref_t<_Up>> && std::is_constructible_v<T, _Up>)
    constexpr explicit(!std::is_convertible_v<_Up, T>)
        expect(_Up &&__u) noexcept(std::is_nothrow_constructible_v<T, _Up>) // strengthened
        : m_has_value(true)
    {
        std::construct_at(std::addressof(m_value_error.m_value), std::forward<_Up>(__u));
    }

    template <class _OtherErr>
        requires std::is_constructible_v<E, const _OtherErr &>
    constexpr explicit(!std::is_convertible_v<const _OtherErr &, E>)
        expect(const error_t<_OtherErr> &err) noexcept(std::is_nothrow_constructible_v<E, const _OtherErr &>) // strengthened
        : m_has_value(false)
    {
        std::construct_at(std::addressof(m_value_error.m_error), err.error());
    }

    template <class _OtherErr>
        requires std::is_constructible_v<E, _OtherErr>
    constexpr explicit(!std::is_convertible_v<_OtherErr, E>)
        expect(error_t<_OtherErr> &&err) noexcept(std::is_nothrow_constructible_v<E, _OtherErr>) // strengthened
        : m_has_value(false)
    {
        std::construct_at(std::addressof(m_value_error.m_error), std::move(err.error()));
    }

    template <class... _Args>
        requires std::is_constructible_v<T, _Args...>
    constexpr explicit expect(std::in_place_t, _Args &&...__args) noexcept(std::is_nothrow_constructible_v<T, _Args...>) // strengthened
        : m_has_value(true)
    {
        std::construct_at(std::addressof(m_value_error.m_value), std::forward<_Args>(__args)...);
    }

    template <class _Up, class... _Args>
        requires std::is_constructible_v<T, std::initializer_list<_Up> &, _Args...>
    constexpr explicit expect(std::in_place_t, std::initializer_list<_Up> __il, _Args &&...__args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<_Up> &, _Args...>) // strengthened
        : m_has_value(true)
    {
        std::construct_at(std::addressof(m_value_error.m_value), __il, std::forward<_Args>(__args)...);
    }

    template <class... _Args>
        requires std::is_constructible_v<E, _Args...>
    constexpr explicit expect(empty_error_t, _Args &&...__args) noexcept(std::is_nothrow_constructible_v<E, _Args...>) // strengthened
        : m_has_value(false)
    {
        std::construct_at(std::addressof(m_value_error.m_error), std::forward<_Args>(__args)...);
    }

    template <class _Up, class... _Args>
        requires std::is_constructible_v<E, std::initializer_list<_Up> &, _Args...>
    constexpr explicit expect(empty_error_t, std::initializer_list<_Up> __il, _Args &&...__args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<_Up> &, _Args...>) // strengthened
        : m_has_value(false)
    {
        std::construct_at(std::addressof(m_value_error.m_error), __il, std::forward<_Args>(__args)...);
    }

    #pragma endregion

    #pragma region destructors

    constexpr ~expect()
        requires(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<E>)
    = default;

    constexpr ~expect()
        requires(!std::is_trivially_destructible_v<T> || !std::is_trivially_destructible_v<E>)
    {
        if (m_has_value)
        {
            std::destroy_at(std::addressof(m_value_error.m_value));
        }
        else
        {
            std::destroy_at(std::addressof(m_value_error.m_error));
        }
    }

    #pragma endregion

    #pragma region assignments
private:
    template <class _T1, class _T2, class... _Args>
    static constexpr void __reinit_expected(_T1 &__newval, _T2 &__oldval, _Args &&...__args)
    {
        if constexpr (std::is_nothrow_constructible_v<_T1, _Args...>)
        {
            std::destroy_at(std::addressof(__oldval));
            std::construct_at(std::addressof(__newval), std::forward<_Args>(__args)...);
        }
        else if constexpr (std::is_nothrow_move_constructible_v<_T1>)
        {
            _T1 __tmp(std::forward<_Args>(__args)...);
            std::destroy_at(std::addressof(__oldval));
            std::construct_at(std::addressof(__newval), std::move(__tmp));
        }
        else
        {
            static_assert(
                std::is_nothrow_move_constructible_v<_T2>,
                "To provide strong exception guarantee, T2 has to satisfy `is_nothrow_move_constructible_v` so that it can "
                "be reverted to the previous state in case an exception is thrown during the assignment.");
            _T2 __tmp(std::move(__oldval));
            std::destroy_at(std::addressof(__oldval));
            auto __trans =
                detail::make_exception_guard([&]
                                            { std::construct_at(std::addressof(__oldval), std::move(__tmp)); });
            std::construct_at(std::addressof(__newval), std::forward<_Args>(__args)...);
            __trans.__complete();
        }
    }

public:
    constexpr expect &operator=(const expect &) = delete;

    constexpr expect &operator=(const expect &__rhs) noexcept(std::is_nothrow_copy_assignable_v<T> &&
                                                                  std::is_nothrow_copy_constructible_v<T> &&
                                                                      std::is_nothrow_copy_assignable_v<E> &&
                                                                          std::is_nothrow_copy_constructible_v<E>) // strengthened
        requires(std::is_copy_assignable_v<T> &&
                 std::is_copy_constructible_v<T> &&
                 std::is_copy_assignable_v<E> &&
                 std::is_copy_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        if (m_has_value && __rhs.m_has_value)
        {
            m_value_error.m_value = __rhs.m_value_error.m_value;
        }
        else if (m_has_value)
        {
            __reinit_expected(m_value_error.m_error, m_value_error.m_value, __rhs.m_value_error.m_error);
        }
        else if (__rhs.m_has_value)
        {
            __reinit_expected(m_value_error.m_value, m_value_error.m_error, __rhs.m_value_error.m_value);
        }
        else
        {
            m_value_error.m_error = __rhs.m_value_error.m_error;
        }
        // note: only reached if no exception+rollback was done inside __reinit_expected
        m_has_value = __rhs.m_has_value;
        return *this;
    }

    constexpr expect &operator=(expect &&__rhs) noexcept(std::is_nothrow_move_assignable_v<T> &&
                                                             std::is_nothrow_move_constructible_v<T> &&
                                                                 std::is_nothrow_move_assignable_v<E> &&
                                                                     std::is_nothrow_move_constructible_v<E>)
        requires(std::is_move_constructible_v<T> &&
                 std::is_move_assignable_v<T> &&
                 std::is_move_constructible_v<E> &&
                 std::is_move_assignable_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        if (m_has_value && __rhs.m_has_value)
        {
            m_value_error.m_value = std::move(__rhs.m_value_error.m_value);
        }
        else if (m_has_value)
        {
            __reinit_expected(m_value_error.m_error, m_value_error.m_value, std::move(__rhs.m_value_error.m_error));
        }
        else if (__rhs.m_has_value)
        {
            __reinit_expected(m_value_error.m_value, m_value_error.m_error, std::move(__rhs.m_value_error.m_value));
        }
        else
        {
            m_value_error.m_error = std::move(__rhs.m_value_error.m_error);
        }
        // note: only reached if no exception+rollback was done inside __reinit_expected
        m_has_value = __rhs.m_has_value;
        return *this;
    }

    template <class _Up = T>
    constexpr expect &operator=(_Up &&__v)
        requires(!std::is_same_v<expect, std::remove_cvref_t<_Up>> &&
                 !is_unexpect_v<std::remove_cvref_t<_Up>> &&
                 std::is_constructible_v<T, _Up> &&
                 std::is_assignable_v<T &, _Up> &&
                 (std::is_nothrow_constructible_v<T, _Up> ||
                  std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        if (m_has_value)
        {
            m_value_error.__val_ = std::forward<_Up>(__v);
        }
        else
        {
            __reinit_expected(m_value_error.m_value, m_value_error.m_error, std::forward<_Up>(__v));
            m_has_value = true;
        }
        return *this;
    }

    template <class _OtherErr>
        requires(detail::can_assign_from_unexpected<T, E, const _OtherErr &>)
    constexpr expect &operator=(const error_t<_OtherErr> &__un)
    {
        if (m_has_value)
        {
            __reinit_expected(m_value_error.m_error, m_value_error.m_value, __un.error());
            m_has_value = false;
        }
        else
        {
            m_value_error.m_error = __un.error();
        }
        return *this;
    }

    template <class _OtherErr>
        requires(detail::can_assign_from_unexpected<T, E, _OtherErr>)
    constexpr expect &operator=(error_t<_OtherErr> &&__un)
    {
        if (m_has_value)
        {
            __reinit_expected(m_value_error.m_error, m_value_error.m_value, std::move(__un.error()));
            m_has_value = false;
        }
        else
        {
            m_value_error.m_error = std::move(__un.error());
        }
        return *this;
    }

    #pragma endregion

    #pragma region emplace

    template <class... _Args>
        requires std::is_nothrow_constructible_v<T, _Args...>
    constexpr T &emplace(_Args &&...__args) noexcept
    {
        if (m_has_value)
        {
            std::destroy_at(std::addressof(m_value_error.m_value));
        }
        else
        {
            std::destroy_at(std::addressof(m_value_error.m_error));
            m_has_value = true;
        }
        return *std::construct_at(std::addressof(m_value_error.m_value), std::forward<_Args>(__args)...);
    }

    template <class _Up, class... _Args>
        requires std::is_nothrow_constructible_v<T, std::initializer_list<_Up> &, _Args...>
    constexpr T &emplace(std::initializer_list<_Up> __il, _Args &&...__args) noexcept
    {
        if (m_has_value)
        {
            std::destroy_at(std::addressof(m_value_error.m_value));
        }
        else
        {
            std::destroy_at(std::addressof(m_value_error.m_error));
            m_has_value = true;
        }
        return *std::construct_at(std::addressof(m_value_error.m_value), __il, std::forward<_Args>(__args)...);
    }

    #pragma endregion

    #pragma region swap

    constexpr void swap(expect &__rhs) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                    std::is_nothrow_swappable_v<T> &&
                                                        std::is_nothrow_move_constructible_v<E> &&
                                                            std::is_nothrow_swappable_v<E>)
        requires(std::is_swappable_v<T> &&
                 std::is_swappable_v<E> &&
                 std::is_move_constructible_v<T> &&
                 std::is_move_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        auto __swap_val_unex_impl = [&](expect &__with_val, expect &__with_err)
        {
            if constexpr (std::is_nothrow_move_constructible_v<E>)
            {
                E __tmp(std::move(__with_err.m_value_error.m_error));
                std::destroy_at(std::addressof(__with_err.m_value_error.m_error));
                auto __trans = std::__make_exception_guard([&]
                                                           { std::construct_at(std::addressof(__with_err.m_value_error.m_error), std::move(__tmp)); });
                std::construct_at(std::addressof(__with_err.m_value_error.m_value), std::move(__with_val.m_value_error.m_value));
                __trans.__complete();
                std::destroy_at(std::addressof(__with_val.m_value_error.m_value));
                std::construct_at(std::addressof(__with_val.m_value_error.m_error), std::move(__tmp));
            }
            else
            {
                static_assert(std::is_nothrow_move_constructible_v<T>,
                              "To provide strong exception guarantee, T has to satisfy `is_nothrow_move_constructible_v` so "
                              "that it can be reverted to the previous state in case an exception is thrown during swap.");
                T __tmp(std::move(__with_val.m_value_error.m_value));
                std::destroy_at(std::addressof(__with_val.m_value_error.m_value));
                auto __trans = std::__make_exception_guard([&]
                                                           { std::construct_at(std::addressof(__with_val.m_value_error.m_value), std::move(__tmp)); });
                std::construct_at(std::addressof(__with_val.m_value_error.m_error), std::move(__with_err.m_value_error.m_error));
                __trans.__complete();
                std::destroy_at(std::addressof(__with_err.m_value_error.m_error));
                std::construct_at(std::addressof(__with_err.m_value_error.m_value), std::move(__tmp));
            }
            __with_val.m_has_value = false;
            __with_err.m_has_value = true;
        };

        if (m_has_value)
        {
            if (__rhs.m_has_value)
            {
                using std::swap;
                swap(m_value_error.m_value, __rhs.m_value_error.m_value);
            }
            else
            {
                __swap_val_unex_impl(*this, __rhs);
            }
        }
        else
        {
            if (__rhs.m_has_value)
            {
                __swap_val_unex_impl(__rhs, *this);
            }
            else
            {
                using std::swap;
                swap(m_value_error.m_error, __rhs.m_value_error.m_error);
            }
        }
    }

    friend constexpr void swap(expect &__x, expect &__y) noexcept(noexcept(__x.swap(__y)))
        requires requires { __x.swap(__y); }
    {
        __x.swap(__y);
    }

    #pragma endregion

    // always present methods
    operator bool() const noexcept { return m_has_value; }

    bool has_value() const noexcept { return m_has_value; }

    // only for E not void
    constexpr const E &error() const & noexcept
    {
        return m_value_error.m_error;
    }

    constexpr E &error() & noexcept
    {
        return m_value_error.m_error;
    }

    constexpr const E &&error() const && noexcept
    {
        return std::move(m_value_error.m_error);
    }

    constexpr E &&error() && noexcept
    {
        return std::move(m_value_error.m_error);
    }

    // only for T not void
    constexpr const T *operator->() const noexcept
    {
        return std::addressof(m_value_error.m_value);
    }

    constexpr T *operator->() noexcept
    {
        return std::addressof(m_value_error.m_value);
    }

    constexpr T &operator*() & noexcept
    {
        return m_value_error.m_value;
    }

    constexpr T &&operator*() && noexcept
    {
        return std::move(m_value_error.m_value);
    }

    constexpr const T &operator*() const & noexcept
    {
        return m_value_error.m_value;
    }

    constexpr const T &&operator*() const && noexcept
    {
        return std::move(m_value_error.m_value);
    }

    constexpr T &value() &
    {
        if (!m_has_value)
            throw new bad_expect_access<E>(error());
        return m_value_error.m_value;
    }

    constexpr T &&value() &&
    {
        if (!m_has_value)
            throw new bad_expect_access<E>(error());
        return std::move(m_value_error.m_value);
    }

    constexpr const T &value() const &
    {
        if (!m_has_value)
            throw new bad_expect_access<E>(error());
        return m_value_error.m_value;
    }

    constexpr const T &&value() const &&
    {
        if (!m_has_value)
            throw new bad_expect_access<E>(error());
        return std::move(m_value_error.m_value);
    }

    template <class U>
    constexpr T value_or(U &&default_value) const & noexcept
    {
        return m_has_value ? **this : static_cast<T>(std::forward<U>(default_value));
    }

    template <class U>
    constexpr T value_or(U &&default_value) && noexcept
    {
        return m_has_value ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
    }

#pragma region Monadic operations

#pragma region and_then
    // and_then
    template <class F>
        requires std::is_invocable_v<F, T> && (is_expect_v<std::invoke_result_t<F, T>> || std::is_same_v<std::invoke_result_t<F, T>, T>)
    constexpr auto and_then(F &&f) &
    {
        return detail::and_then_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto and_then(F &&f) const &
    {
        return detail::and_then_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto and_then(F &&f) &&
    {
        return detail::and_then_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr auto and_then(F &&f) const &&
    {
        return detail::and_then_impl(std::move(*this), std::forward<F>(f));
    }

#pragma endregion

#pragma region transform

    // transform
    template <class F>
    constexpr auto transform(F &&f) &
    {
        return detail::transform_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform(F &&f) const &
    {
        return detail::transform_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform(F &&f) &&
    {
        return detail::transform_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform(F &&f) const &&
    {
        return detail::transform_impl(std::move(*this), std::forward<F>(f));
    }

#pragma endregion

#pragma region or_else

    // transform
    template <class F>
    constexpr auto or_else(F &&f) &
    {
        return detail::or_else_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto or_else(F &&f) const &
    {
        return detail::or_else_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto or_else(F &&f) &&
    {
        return detail::or_else_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr auto or_else(F &&f) const &&
    {
        return detail::or_else_impl(std::move(*this), std::forward<F>(f));
    }

#pragma endregion

#pragma region transform_error

    // transform error
    template <class F>
    constexpr auto transform_error(F &&f) &
    {
        return detail::transform_error_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform_error(F &&f) const &
    {
        return detail::transform_error_impl(*this, std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform_error(F &&f) &&
    {
        return detail::transform_error_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr auto transform_error(F &&f) const &&
    {
        return detail::transform_error_impl(std::move(*this), std::forward<F>(f));
    }

#pragma endregion

#pragma endregion

private:
    struct __empty_t
    {
    };
    // replace with macro for msvc support
    [[no_unique_address]] union __union_t
    {
        constexpr __union_t() : __empty_() {}

        constexpr ~__union_t()
            requires(std::is_trivially_destructible_v<T> && std::is_trivially_destructible_v<E>)
        = default;

        // the expected's destructor handles this
        constexpr ~__union_t()
            requires(!std::is_trivially_destructible_v<T> || !std::is_trivially_destructible_v<E>)
        {
        }

        [[no_unique_address]] __empty_t __empty_;
        [[no_unique_address]] T m_value;
        [[no_unique_address]] E m_error;
    } m_value_error;

    bool m_has_value{false};
};
