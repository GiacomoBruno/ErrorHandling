#pragma once
#include "expect_base.h"

template<class T, class E>
requires std::is_void_v<T> && std::is_void_v<E>
struct expect<T,E>
{

    //default constructors
    constexpr expect() noexcept : m_has_value{true} {}
    constexpr expect(empty_error_t) noexcept :  m_has_value{false} {}
    
    //error constructors
    constexpr expect(const empty_error_t&) noexcept : m_has_value{false}{}
    constexpr expect(empty_error_t&&) noexcept : m_has_value{false} {}

    constexpr expect& operator=(const empty_error_t&) noexcept
    {
        m_has_value = false;
    }

    constexpr expect& operator=(empty_error_t&&) noexcept
    {
        m_has_value = false;
        return *this;
    }

    //value constructors
    constexpr expect(const empty_value_t&) noexcept :  m_has_value{true}{}
    constexpr expect(empty_value_t&&) noexcept : m_has_value{true} {}


    constexpr expect& operator=(const empty_value_t&) noexcept
    {
        m_has_value = true;
        return *this;
    }

    constexpr expect& operator=(empty_value_t&&) noexcept
    {
        m_has_value = true;
        return *this;
    }


    //copy constructor and assignment
    constexpr expect(const expect& other) noexcept : m_has_value{other.m_has_value} {}
    constexpr expect& operator=(const expect& other) noexcept {
        m_has_value = other.m_has_value;
        return *this;
    }

    //move constructor and assignment
    constexpr expect(expect&& other) noexcept : m_has_value{other.m_has_value} {}
    constexpr expect& operator=(expect&& other) noexcept
    {
        using std::swap;
        swap(m_has_value, other.m_has_value);
        return *this;
    }

    //in place constructors is nonsense with void T

    //always present methods
    operator bool() const noexcept { return m_has_value; }

    bool has_value() const noexcept { return m_has_value; }

    constexpr void swap(expect<T,E>& other) noexcept(std::is_nothrow_move_constructible_v<E> && std::is_nothrow_swappable_v<E>)
    {
        using std::swap;
        swap(m_has_value, other.m_has_value); //swap the flag
        //swap of m_value_error is unnecessary since they are both void anyway
        //swap(m_value_error, other.m_value_error); //swap the values / errors
    }

    //only for E not void
    constexpr const void error() const& noexcept
    {
    } 

    constexpr void error() & noexcept
    {
    } 

    constexpr const void error() const&& noexcept
    {
    } 

    constexpr void error() && noexcept  
    {
    } 

    //only for T not void
    constexpr void operator->() &
    {
    }

    constexpr void operator->() &&
    {
    }

    constexpr const void operator->() const&
    {
    }

    constexpr const void operator->() const&&
    {
    }

    constexpr void operator*() &
    {
    }

    constexpr void operator*() &&
    {
    }

    constexpr const void operator*() const&
    {
    }

    constexpr const void operator*() const&&
    {
    }

    constexpr void value() &
    {
        if(!m_has_value) throw new bad_expect_access<E>(error());
    }

    constexpr void value() &&
    {
        if(!m_has_value) throw new bad_expect_access<E>(error());
    }

    constexpr const void value() const&
    {
        if(!m_has_value) throw new bad_expect_access<E>(error());
    }

    constexpr const void value() const&&
    {
        if(!m_has_value) throw new bad_expect_access<E>(error());
    }
    
    //value or is nonsense with void type
    

    #pragma region Monadic operations

    #pragma region and_then
    //and_then
    template<class F>
        requires std::is_invocable_v<F, T> && (is_expect_v<std::invoke_result_t<F, T>> || std::is_same_v<std::invoke_result_t<F, T>, T>)
    constexpr auto and_then( F&& f ) &
    {
        return detail::and_then_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto and_then(F&& f) const&
    {
        return detail::and_then_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto and_then( F&& f) &&
    {
        return detail::and_then_impl(std::move(*this), std::forward<F>(f));
    }

    template<class F>
    constexpr auto and_then( F&& f) const&&
    {
        return detail::and_then_impl(std::move(*this), std::forward<F>(f));
    }

    #pragma endregion

    #pragma region transform

    //transform
    template<class F>
    constexpr auto transform( F&& f ) &
    {
        return detail::transform_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform(F&& f) const&
    {
        return detail::transform_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform( F&& f) &&
    {
        return detail::transform_impl(std::move(*this), std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform( F&& f) const&&
    {
        return detail::transform_impl(std::move(*this), std::forward<F>(f));
    }


    #pragma endregion

    #pragma region or_else

    //transform
    template<class F>
    constexpr auto or_else( F&& f ) &
    {
        return detail::or_else_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto or_else(F&& f) const&
    {
        return detail::or_else_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto or_else( F&& f) &&
    {
        return detail::or_else_impl(std::move(*this), std::forward<F>(f));
    }

    template<class F>
    constexpr auto or_else( F&& f) const&&
    {
        return detail::or_else_impl(std::move(*this), std::forward<F>(f));
    }

    #pragma endregion

    #pragma region transform_error

    //transform error
    template<class F>
    constexpr auto transform_error( F&& f ) &
    {
        return detail::transform_error_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform_error(F&& f) const&
    {
        return detail::transform_error_impl(*this, std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform_error( F&& f) &&
    {
        return detail::transform_error_impl(std::move(*this), std::forward<F>(f));
    }

    template<class F>
    constexpr auto transform_error( F&& f) const&&
    {
        return detail::transform_error_impl(std::move(*this), std::forward<F>(f));
    }
    
    #pragma endregion

    #pragma endregion



private:
    bool m_has_value{false};
};