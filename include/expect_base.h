#pragma once
#include <exception>
#include <type_traits>
//#include <expected>
namespace detail
{

template <class _Rollback>
struct exception_guard {
  exception_guard() = delete;

  constexpr explicit exception_guard(_Rollback __rollback)
      : __rollback_(std::move(__rollback)), __completed_(false) {}

  constexpr
  exception_guard(exception_guard&& __other)
      noexcept(std::is_nothrow_move_constructible_v<_Rollback>)
      : __rollback_(std::move(__other.__rollback_)), __completed_(__other.__completed_) {
    __other.__completed_ = true;
  }

  exception_guard(exception_guard const&)            = delete;
  exception_guard& operator=(exception_guard const&) = delete;
  exception_guard& operator=(exception_guard&&)      = delete;

  constexpr void __complete() noexcept { __completed_ = true; }

  constexpr ~exception_guard() {
    if (!__completed_)
      __rollback_();
  }

private:
  _Rollback __rollback_;
  bool __completed_;
};


template <class _Rollback>
constexpr exception_guard<_Rollback> make_exception_guard(_Rollback __rollback) {
  return exception_guard<_Rollback>(std::move(__rollback));
}


template<class E>
struct unexpected{
    using unexpected_type = E;
};


template<class E> 
    requires (!std::is_void_v<E>) && (!std::is_copy_constructible_v<E>) && (!std::is_move_constructible_v<E>)
struct unexpected<E>
{
public:
  unexpected() = delete;

  template <class... Args>
    requires std::is_constructible_v<E, Args&&...>
  constexpr explicit unexpected(Args &&...args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
  : m_val{std::forward<Args>(args)...} {}

  template <class U, class... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>
  constexpr explicit unexpected(std::initializer_list<U> l, Args &&...args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>)
  : m_val(l, std::forward<Args>(args)...) {}

  constexpr const E &value() const & { return m_val; }
  constexpr E &value() & { return m_val; }
  constexpr E &&value() && { return std::move(m_val); }
  constexpr const E &&value() const && { return std::move(m_val); }

private:
  E m_val;
};

template<class E> 
    requires (!std::is_void_v<E>) && (!std::is_move_constructible_v<E>) && std::is_copy_constructible_v<E>
struct unexpected<E>
{
public:
  unexpected() = delete;

  //requires copiable
  constexpr explicit unexpected(const E &e) noexcept(std::is_nothrow_copy_constructible_v<E>)
  : m_val(e) {}

  template <class... Args>
    requires std::is_constructible_v<E, Args&&...>
  constexpr explicit unexpected(Args &&...args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
  : m_val{std::forward<Args>(args)...} {}

  template <class U, class... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>
  constexpr explicit unexpected(std::initializer_list<U> l, Args &&...args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>)
  : m_val(l, std::forward<Args>(args)...) {}

  constexpr const E &value() const & { return m_val; }
  constexpr E &value() & { return m_val; }
  constexpr E &&value() && { return std::move(m_val); }
  constexpr const E &&value() const && { return std::move(m_val); }

private:
  E m_val;
};


template<class E> 
    requires (!std::is_void_v<E>) && (!std::is_copy_constructible_v<E>) && std::is_move_constructible_v<E>
struct unexpected<E>
{
public:
  unexpected() = delete;

  //requires moveable
  constexpr explicit unexpected(E &&e) noexcept(std::is_nothrow_move_constructible_v<E>)
  : m_val(std::move(e)) {}

  template <class... Args>
    requires std::is_constructible_v<E, Args&&...>
  constexpr explicit unexpected(Args &&...args) noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
  : m_val{std::forward<Args>(args)...} {}

  template <class U, class... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>
  constexpr explicit unexpected(std::initializer_list<U> l, Args &&...args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>)
  : m_val(l, std::forward<Args>(args)...) {}

  constexpr const E &value() const & { return m_val; }
  constexpr E &value() & { return m_val; }
  constexpr E &&value() && { return std::move(m_val); }
  constexpr const E &&value() const && { return std::move(m_val); }

private:
  E m_val;
};

template<class E> 
    requires (!std::is_void_v<E>) && std::is_copy_constructible_v<E> && std::is_move_constructible_v<E>
struct unexpected<E>
{
public:
  unexpected() = delete;
  //requires copiable
  constexpr explicit unexpected(const E &e) : m_val(e) {}

  //requires moveable
  constexpr explicit unexpected(E &&e) : m_val(std::move(e)) {}

  template <class... Args>
    requires std::is_constructible_v<E, Args&&...>
  constexpr explicit unexpected(Args &&...args)  noexcept(std::is_nothrow_constructible_v<E, Args&&...>)
  : m_val{std::forward<Args>(args)...} {}

  template <class U, class... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>
  constexpr explicit unexpected(std::initializer_list<U> l, Args &&...args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>)
  : m_val(l, std::forward<Args>(args)...) {}

  constexpr const E &value() const & { return m_val; }
  constexpr E &value() & { return m_val; }
  constexpr E &&value() && { return std::move(m_val); }
  constexpr const E &&value() const && { return std::move(m_val); }

private:
  E m_val;
};

template<class E>
    requires std::is_void_v<E>
struct unexpected<E>
{
    struct do_not_use {};
    constexpr explicit unexpected(do_not_use, do_not_use) noexcept {}

    constexpr const void value() const & { }
    constexpr void value() & { }
    constexpr void value() && { }
    constexpr const void value() const && { }

};


struct expected_void
{
    struct do_not_use {};
    constexpr explicit expected_void(do_not_use, do_not_use) noexcept {}
};

}

template<class T>
using error_t = detail::unexpected<T>;

using empty_error_t = error_t<void>;
using empty_value_t = detail::expected_void;

static constexpr empty_error_t error_v{empty_error_t::do_not_use{}, empty_error_t::do_not_use{}};
static constexpr empty_value_t value_v{empty_value_t::do_not_use{}, empty_value_t::do_not_use{}};

template <class _Err>
class bad_expect_access;

template <>
class bad_expect_access<void> : public std::exception {
public:
  bad_expect_access() noexcept                           = default;
  bad_expect_access(const bad_expect_access&)            = default;
  bad_expect_access(bad_expect_access&&)                 = default;
  bad_expect_access& operator=(const bad_expect_access&) = default;
  bad_expect_access& operator=(bad_expect_access&&)      = default;
  ~bad_expect_access() override                          = default;

public:
  // The way this has been designed (by using a class template below) means that we'll already
  // have a profusion of these vtables in TUs, and the dynamic linker will already have a bunch
  // of work to do. So it is not worth hiding the <void> specialization in the dylib, given that
  // it adds deployment target restrictions.
  const char* what() const noexcept override { return "bad access to expect"; }
};

template <class _Err>
class bad_expect_access : public bad_expect_access<void> {
public:
   explicit bad_expect_access(_Err __e) : __unex_(std::move(__e)) {}

   _Err& error() & noexcept { return __unex_; }
   const _Err& error() const& noexcept { return __unex_; }
   _Err&& error() && noexcept { return std::move(__unex_); }
   const _Err&& error() const&& noexcept { return std::move(__unex_); }

private:
  _Err __unex_;
};

template<class T, class E>
struct expect
{
};

namespace detail
{
template<class T>
struct is_expect : std::false_type{};

template <class T, class E>
struct is_expect<expect<T, E>> : std::true_type {};

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
struct expect_value<expect<T,E>>
{
    using value_type = T;
};

template<class T>
struct expect_error
{
    using error_type = void;
};

template<class T, class E>
struct expect_error<expect<T,E>>
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
  using type = expect<std::invoke_result_t<F, expect_value_t<Exp>>, expect_error_t<Exp>>;
};

template<class Exp, class F>
requires std::is_invocable_v<F>
struct vtransformed_expect<Exp,F>
{
  using type = expect<std::invoke_result_t<F>, expect_error_t<Exp>>;
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
  using type = expect<expect_value_t<Exp>,std::invoke_result_t<F, expect_error_t<Exp>>>;
};

template<class Exp, class F>
requires std::is_invocable_v<F>
struct etransformed_expect<Exp,F>
{
  using type = expect<expect_value_t<Exp>,std::invoke_result_t<F>>;
};


  template <class T, class E, class _Up, class _OtherErr, class _UfQual, class _OtherErrQual>
  using can_convert =
      std::conjunction<std::is_constructible<T, _UfQual>,
            std::is_constructible<E, _OtherErrQual>,
            std::negation<std::is_constructible<T, expect<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<T, expect<_Up, _OtherErr>>>,
            std::negation<std::is_constructible<T, const expect<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<T, const expect<_Up, _OtherErr>>>,
            std::negation<std::is_convertible<expect<_Up, _OtherErr>&, T>>,
            std::negation<std::is_convertible<expect<_Up, _OtherErr>&&, T>>,
            std::negation<std::is_convertible<const expect<_Up, _OtherErr>&, T>>,
            std::negation<std::is_convertible<const expect<_Up, _OtherErr>&&, T>>,
            std::negation<std::is_constructible<unexpected<E>, expect<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, expect<_Up, _OtherErr>>>,
            std::negation<std::is_constructible<unexpected<E>, const expect<_Up, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, const expect<_Up, _OtherErr>>> >;

  template <class T, class _Up, class _UfQual>
  using can_convert_void_error =
      std::conjunction<std::is_constructible<T, _UfQual>,
            std::negation<std::is_constructible<T, expect<_Up, void>&>>,
            std::negation<std::is_constructible<T, expect<_Up, void>>>,
            std::negation<std::is_constructible<T, const expect<_Up, void>&>>,
            std::negation<std::is_constructible<T, const expect<_Up, void>>>,
            std::negation<std::is_convertible<expect<_Up, void>&, T>>,
            std::negation<std::is_convertible<expect<_Up, void>&&, T>>,
            std::negation<std::is_convertible<const expect<_Up, void>&, T>>,
            std::negation<std::is_convertible<const expect<_Up, void>&&, T>>>;

  template <class E, class _OtherErr, class _OtherErrQual>
  using can_convert_void_value =
      std::conjunction<
            std::is_constructible<E, _OtherErrQual>,
            std::negation<std::is_constructible<unexpected<E>, expect<void, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, expect<void, _OtherErr>>>,
            std::negation<std::is_constructible<unexpected<E>, const expect<void, _OtherErr>&>>,
            std::negation<std::is_constructible<unexpected<E>, const expect<void, _OtherErr>>> >;


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

namespace detail
{
    template<class Exp, class F>
    requires (!std::is_void_v<expect_value_t<Exp>>)
    constexpr std::decay_t<Exp> and_then_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_value_t<Exp>>)
    {  
        if(std::forward<Exp>(exp).has_value()) 
          return std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));
          
        return std::forward<Exp>(exp);
    }

    template<class Exp, class F>
    requires std::is_void_v<expect_value_t<Exp>>
    constexpr std::decay_t<Exp> and_then_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F>)
    {
      if(std::forward<Exp>(exp).has_value()) 
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
      requires (!std::is_void_v<expect_value_t<Exp>>)
    constexpr value_transformed_t<Exp, F> transform_impl(Exp&& exp, F&& f)
    {
      using result_t = value_transformed_t<Exp, F>;
      using r_value_t = expect_value_t<result_t>;

      if(std::forward<Exp>(exp).has_value()) 
      {
        if constexpr (std::is_void_v<r_value_t>) //if T -> void then return value_v
        {
          std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));
          return value_v;
        }
        else // if T -> nonvoid or F -> expect<void, error> then return F call  directly
        {
          return std::invoke(std::forward<F>(f), *std::forward<Exp>(exp));
        }
      }

      if constexpr (std::is_void_v<expect_error_t<Exp>>)
      {
        return error_v;
      }
      else
      {
        return result_t{error_v, std::forward<Exp>(exp).error()};
      }
    }

    template<class Exp, class F>
    requires std::is_void_v<expect_value_t<Exp>>
    constexpr value_transformed_t<Exp, F> transform_impl(Exp&& exp, F&& f)
    {
      using result_t = value_transformed_t<Exp, F>;
      using r_value_t = expect_value_t<result_t>;

      if(std::forward<Exp>(exp).has_value()) 
      {
        if constexpr (std::is_void_v<r_value_t>) //if void -> void then return value_v
        {
          std::invoke(std::forward<F>(f));
          return value_v;
        }
        else // if void -> nonvoid or F -> expect<void, error> then return F call  directly
        {
          return std::invoke(std::forward<F>(f));
        }
      }
      
      if constexpr (std::is_void_v<expect_error_t<Exp>>)
      {
        return error_v;
      }
      else
      {
        return result_t{error_v, std::forward<Exp>(exp).error()};
      }
    }

    template<class Exp, class F>
    requires (!std::is_void_v<expect_error_t<Exp>>)
    constexpr std::decay_t<Exp> or_else_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_error_t<Exp>>)
    {  
        if(!std::forward<Exp>(exp).has_value()) 
          return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
          
        return std::forward<Exp>(exp);
    }

    template<class Exp, class F>
    requires std::is_void_v<expect_error_t<Exp>>
    constexpr std::decay_t<Exp> or_else_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F>)
    {
      if(!std::forward<Exp>(exp).has_value()) 
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
    requires (!std::is_void_v<expect_error_t<Exp>>)
    constexpr error_transformed_t<Exp, F> transform_error_impl(Exp&& exp, F&& f) noexcept(std::is_nothrow_invocable_v<F, expect_error_t<Exp>>)
    {
      using result_t = error_transformed_t<Exp, F>;
      using r_error_t = expect_error_t<result_t>;

      if(!std::forward<Exp>(exp).has_value()) 
      {
        if constexpr (std::is_void_v<r_error_t>) //if E -> void then return value_v
        {
          std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
          return error_v;
        }
        else // if E -> nonvoid or F -> expect<void, error> then return F call  directly
        {
          return std::invoke(std::forward<F>(f), std::forward<Exp>(exp).error());
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

      if(!std::forward<Exp>(exp).has_value()) 
      {
        if constexpr (std::is_void_v<r_error_t>) //if void -> void then return value_v
        {
          std::invoke(std::forward<F>(f));
          return error_v;
        }
        else // if void -> nonvoid or F -> expect<void, error> then return F call  directly
        {
          return result_t{error_v, std::invoke(std::forward<F>(f))};
        }
      }
      return *std::forward<Exp>(exp);
    }

}


