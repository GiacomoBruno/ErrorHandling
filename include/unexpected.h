#pragma once
#include <type_traits>

namespace gb {

    /*
template<class E>
struct unexpected{};


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

template<class T>
unexpected(const T&) -> unexpected<T const&>;

template<class T>
unexpected(T&&) -> unexpected<T && >;

template<class T>
unexpected(T) -> unexpected<T>;

*/

template<class E>
struct unexpected
{
public:
    static_assert(!std::is_same<E, void>::value, "E must not be void");

    unexpected() = delete;
    constexpr explicit unexpected(const E& e)
        : m_val{e}
    {
    }

    constexpr explicit unexpected(E&& e)
        : m_val{std::move(e)}
    {
    }

    template<class... Args, typename std::enable_if<std::is_constructible<E, Args&&...>::value>::type* = nullptr>
    constexpr explicit unexpected(Args&&... args)
        : m_val{std::forward<Args>(args)...}
    {
    }
    template<class U, class... Args, typename std::enable_if<std::is_constructible<E, std::initializer_list<U>&, Args&&...>::value>::type* = nullptr>
    constexpr explicit unexpected(std::initializer_list<U> l, Args&&... args)
        : m_val{l, std::forward<Args>(args)...}
    {
    }

    constexpr const E& error() const& { return m_val; }
    constexpr E& error() & { return m_val; }
    constexpr E&& error() && { return std::move(m_val); }
    constexpr const E&& error() const&& { return std::move(m_val); }

private:
    E m_val;
};


struct unexpected_void
{
    struct do_not_use {};
    constexpr explicit unexpected_void(do_not_use, do_not_use) noexcept {}
};

struct expected_void
{
    struct do_not_use {};
    constexpr explicit expected_void(do_not_use, do_not_use) noexcept {}
};

template<class E>
unexpected(E) -> unexpected<E>;


}
