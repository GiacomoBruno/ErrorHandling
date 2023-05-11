#pragma once

#include "unexpected.h"
#include "bad_expected_access.h"
#include "exception_guard.h"

namespace gb
{

template<class T, class E>
struct expected
{
};


template<class T, class E, class U, class F>
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs){ return false;}

template<class T, class E, class U, class F> //non void types
    requires (!std::is_void_v<T>) && (!std::is_void_v<E>) && (!std::is_void_v<U>) && (!std::is_void_v<F>)
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? false : (!lhs.has_value() ? lhs.error() == rhs.error() : *lhs == *rhs);
}

template<class T, class E, class U, class F> //void T or U but not both
    requires (std::is_void_v<T> || std::is_void_v<U>) && (!std::is_same_v<T, U>) && (!std::is_void_v<E>) && (!std::is_void_v<F>)
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? false : (!lhs.has_value() ? lhs.error() == rhs.error() : false);
}

template<class T, class E, class U, class F> //void E or F but not both
    requires (std::is_void_v<E> || std::is_void_v<F>) && (!std::is_same_v<E, F>) && (!std::is_void_v<T>)&& (!std::is_void_v<U>)
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? false : (!lhs.has_value() ? false : *lhs == *rhs);
}

template<class T, class E, class U, class F> //void E or F and void T or U
    requires (std::is_void_v<E> || std::is_void_v<F>) && (!std::is_same_v<E, F>) &&
             (std::is_void_v<T> || std::is_void_v<U>) && (!std::is_same_v<T, U>)
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return false; //the types are not comparable 
}

template<class T, class E, class U, class F> //all void
    requires std::is_void_v<E> && std::is_void_v<F> && std::is_void_v<T> && std::is_void_v<U>
constexpr bool operator==(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return lhs.has_value() == rhs.has_value(); //true if both are error or value
}

template<class T, class E, class U, class F>
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs){ return false; }

template<class T, class E, class U, class F>//all non void
    requires (!std::is_void_v<T>) && (!std::is_void_v<E>) && (!std::is_void_v<U>) && (!std::is_void_v<F>)
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? true : (!lhs.has_value() ? lhs.error() != rhs.error() : *lhs != *rhs);
}

template<class T, class E, class U, class F> //void T or U but not both
    requires (std::is_void_v<T> || std::is_void_v<U>) && (!std::is_same_v<T, U>) && (!std::is_void_v<E>) && (!std::is_void_v<F>)
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? true : (!lhs.has_value() ? lhs.error() != rhs.error() : true);
}


template<class T, class E, class U, class F> //void E or F but not both
    requires (std::is_void_v<E> || std::is_void_v<F>) && (!std::is_same_v<E, F>) && (!std::is_void_v<T>)&& (!std::is_void_v<U>)
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return (lhs.has_value() != rhs.has_value()) ? true : (!lhs.has_value() ? true : *lhs == *rhs);
}

template<class T, class E, class U, class F> //void E or F and void T or U
    requires (std::is_void_v<E> || std::is_void_v<F>) && (!std::is_same_v<E, F>) &&
             (std::is_void_v<T> || std::is_void_v<U>) && (!std::is_same_v<T, U>)
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return true; //the types are not comparable 
}

template<class T, class E, class U, class F> //all void
    requires std::is_void_v<E> && std::is_void_v<F> && std::is_void_v<T> && std::is_void_v<U>
constexpr bool operator!=(const expected<T, E>& lhs, const expected<U, F>& rhs)
{
    return lhs.has_value() != rhs.has_value(); //false if both are error or value
}


template<class T, class E, class U>
requires (!std::is_void_v<T>)
constexpr bool operator==(const expected<T, E>& x, const U& v)
{
    return x.has_value() ? *x == v : false;
}

template<class T, class E, class U>
requires (!std::is_void_v<T>)
constexpr bool operator==(const U& v, const expected<T, E>& x)
{
    return x.has_value() ? *x == v : false;
}

template<class T, class E, class U>
requires (!std::is_void_v<T>)
constexpr bool operator!=(const expected<T, E>& x, const U& v)
{
    return x.has_value() ? *x != v : true;
}

template<class T, class E, class U>
requires (!std::is_void_v<T>)
constexpr bool operator!=(const U& v, const expected<T, E>& x)
{
    return x.has_value() ? *x != v : true;
}






}

