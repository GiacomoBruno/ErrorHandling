#pragma once

#include "expected_base.h"

namespace gb
{
    
template<class T, class E>
    requires std::is_void_v<T> && std::is_void_v<E>
struct expected<T, E> 
{
    constexpr expected() noexcept : m_has_value{true} {}
    constexpr expected(nullopt_t) noexcept : m_has_value{false} {}

    constexpr expected(const expected& other) noexcept : m_has_value{other.m_has_value} {}
    constexpr expected(expected&& other) noexcept : m_has_value{other.m_has_value} {};

    auto& operator=(const expected& other) noexcept 
    {
        m_has_value = other.m_has_value;
        return *this;
    }

    template<class T2, class E2>
    auto& operator=(const expected<T2, E2>& other) noexcept
    {
        m_has_value = other.has_value();
        return *this;
    }

    auto& operator=(expected&& other) noexcept
    {
        m_has_value = other.m_has_value;
        return *this;
    }

    template<class T2, class E2>
    auto& operator=(expected<T2, E2>&& other) noexcept
    {
        m_has_value = other.has_value();
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return this->m_has_value;
    }

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return this->m_has_value;
    }

    [[nodiscard]] constexpr auto operator<=>(const expected& other) noexcept
    {
        return m_has_value <=> other.m_has_value;
    }

    template<class T2, class E2>
    [[nodiscard]] constexpr auto operator==(const expected<T2, E2>& other)
    {
        return !m_has_value && (m_has_value == other.has_value());
    }

    template<class T2, class E2>
    [[nodiscard]] constexpr auto operator!=(const expected<T2, E2>& other)
    {
        return m_has_value || other.has_value();
    }



private:
    bool m_has_value;

};
}