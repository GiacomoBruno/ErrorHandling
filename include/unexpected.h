#pragma once

#include <type_traits>

namespace ik::av
{

template<class E>
struct unexpected{
    using unexpected_type = E;
};

template<class E> 
    requires (!std::is_void_v<E>)
struct unexpected<E>
{
public:
  unexpected() = delete;
  constexpr explicit unexpected(const E &e) : m_val(e) {}

  constexpr explicit unexpected(E &&e) : m_val(std::move(e)) {}

  template <class... Args>
    requires std::is_constructible_v<E, Args&&...>
  constexpr explicit unexpected(Args &&...args) : m_val{std::forward<Args>(args)...} {}

  template <class U, class... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args&&...>
  constexpr explicit unexpected(std::initializer_list<U> l, Args &&...args) : m_val(l, std::forward<Args>(args)...) {}

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
};

using nullopt_t = unexpected<void>;

static constexpr nullopt_t nullopt{nullopt_t::do_not_use{},
                                   nullopt_t::do_not_use{}};
}
