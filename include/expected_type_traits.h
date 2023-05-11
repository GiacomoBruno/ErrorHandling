#pragma once
#include <type_traits>
#include "expected_base.h"
#include "unexpected.h"

namespace gb {

using unexpect_t = unexpected_void;
using expect_t = expected_void;

static constexpr unexpect_t unexpect{unexpect_t::do_not_use{}, unexpect_t::do_not_use{}};
static constexpr expect_t expect{expect_t::do_not_use{}, expect_t::do_not_use{}};

//additional type-traits for expected, unexpected
namespace detail
{
template<class T>
struct is_expect : std::false_type{};

template <class T, class E>
struct is_expect<expected<T, E>> : std::true_type {};

template<class T>
struct is_unexpected : std::false_type{};

template <class E>
struct is_unexpected<unexpected<E>> : std::true_type {};


template<class T>
struct expect_value 
{
    using value_type = void;
};

template<class T, class E>
struct expect_value<expected<T,E>>
{
    using value_type = T;
};

template<class T>
struct expect_error
{
    using error_type = void;
};

template<class T, class E>
struct expect_error<expected<T,E>>
{
    using error_type = E;
};

template<class T>
using expect_value_t = detail::expect_value<std::decay_t<T>>::value_type;

template<class T>
using expect_error_t = detail::expect_error<std::decay_t<T>>::error_type;

template<class Exp, class F>
struct vtransformed_expect
{
  using type = void;
};

template<class Exp, class F>
requires std::is_invocable_v<F, expect_value_t<Exp>>
struct vtransformed_expect<Exp,F>
{
  using type = expected<std::invoke_result_t<F, expect_value_t<Exp>>, expect_error_t<Exp>>;
};

template<class Exp, class F>
requires std::is_invocable_v<F>
struct vtransformed_expect<Exp,F>
{
  using type = expected<std::invoke_result_t<F>, expect_error_t<Exp>>;
};


template<class Exp, class F>
struct etransformed_expect
{
  using type = void;
};

template<class Exp, class F>
requires std::is_invocable_v<F, expect_error_t<Exp>>
struct etransformed_expect<Exp,F>
{
  using type = expected<expect_value_t<Exp>,std::invoke_result_t<F, expect_error_t<Exp>>>;
};

template<class Exp, class F>
requires std::is_invocable_v<F>
struct etransformed_expect<Exp,F>
{
  using type = expected<expect_value_t<Exp>,std::invoke_result_t<F>>;
};


  template <class T, class E, class _Up, class _OtherErr, class _UfQual, class _OtherErrQual>
  using can_convert =
      std::conjunction<std::is_constructible<T, _UfQual>,
            std::is_constructible<E, _OtherErrQual>,
            std::negation<std::is_constructible<T, expected<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<T, expected<_Up, _OtherErr>>>,
            std::negation<std::is_constructible<T, const expected<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<T, const expected<_Up, _OtherErr>>>,
            std::negation<std::is_convertible<expected<_Up, _OtherErr>&, T>>,
            std::negation<std::is_convertible<expected<_Up, _OtherErr>&&, T>>,
            std::negation<std::is_convertible<const expected<_Up, _OtherErr>&, T>>,
            std::negation<std::is_convertible<const expected<_Up, _OtherErr>&&, T>>,
            std::negation<std::is_constructible<unexpected<E>, expected<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, expected<_Up, _OtherErr>>>,
            std::negation<std::is_constructible<unexpected<E>, const expected<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, const expected<_Up, _OtherErr>>> >;

  template <class T, class _Up, class _UfQual>
  using can_convert_void_error =
      std::conjunction<std::is_constructible<T, _UfQual>,
            std::negation<std::is_constructible<T, expected<_Up, void>&>>,
            std::negation<std::is_constructible<T, expected<_Up, void>>>,
            std::negation<std::is_constructible<T, const expected<_Up, void>&>>,
            std::negation<std::is_constructible<T, const expected<_Up, void>>>,
            std::negation<std::is_convertible<expected<_Up, void>&, T>>,
            std::negation<std::is_convertible<expected<_Up, void>&&, T>>,
            std::negation<std::is_convertible<const expected<_Up, void>&, T>>,
            std::negation<std::is_convertible<const expected<_Up, void>&&, T>>>;

  template <class E, class _OtherErr, class _OtherErrQual>
  using can_convert_void_value =
      std::conjunction<
            std::is_constructible<E, _OtherErrQual>,
            std::negation<std::is_constructible<unexpected<E>, expected<void, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, expected<void, _OtherErr>>>,
            std::negation<std::is_constructible<unexpected<E>, const expected<void, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, const expected<void, _OtherErr>>> >;


  template <template <class...> class _Func, class ..._Args>
  struct lazy : _Func<_Args...> {};


  template <class T, class E, class _OtherErrQual>
  static constexpr bool can_assign_from_unexpected =
      std::conjunction<std::is_constructible<E, _OtherErrQual>,
            std::is_assignable<E&, _OtherErrQual>,
            lazy<std::disjunction,
                  std::is_nothrow_constructible<E, _OtherErrQual>,
                  std::is_nothrow_move_constructible<T>,
                  std::is_nothrow_move_constructible<E>> >::value;

}

template <class T> 
using is_expected = detail::is_expect<std::decay_t<T>>;

template <class T> 
using is_unexpected = detail::is_unexpected<std::decay_t<T>>;

template<class T>
constexpr static bool is_expect_v = is_expected<T>::value;

template<class T>
constexpr static bool is_unexpect_v = is_unexpected<T>::value;

template<class T>
using expect_value_t = detail::expect_value_t<T>;

template<class T>
using expect_error_t = detail::expect_error_t<T>;

template<class Exp, class F>
using value_transformed_t = detail::vtransformed_expect<Exp, F>::type;

template<class Exp, class F>
using error_transformed_t = detail::etransformed_expect<Exp, F>::type;
}
