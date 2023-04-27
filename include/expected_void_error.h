#pragma once
#include <type_traits>
#include <optional>
#include "expected_base.h"

namespace gb
{

template<class T, class E>
    requires std::is_void_v<E>
struct expected<T, E> : private detail::control<detail::expected_construct_base<T,E>, T>
{
    using MyBase = detail::control<detail::expected_construct_base<T,E>, T>;

//this is equivalent to a std::optional
    static_assert(!(std::is_same_v<std::remove_cv_t<T>, nullopt_t> || std::is_same_v<std::remove_cv_t<T>, std::in_place_t>),
        "T in expected<T> must be a type other than nullopt_t or in_place_t.");
    static_assert(std::is_object_v<T> && std::is_destructible_v<T> && !std::is_array_v<T>,
        "T in expected<T> must meet the Cpp17Destructible requirements.");

    using value_type = T;
    using error_type = E;

#pragma region CONSTRUCTORS
    constexpr expected() noexcept {}
    constexpr expected(nullopt_t) noexcept {}

    template <class... Args>
        requires std::is_constructible_v<T, Args>
    constexpr explicit expected(std::in_place_t, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : MyBase(std::in_place, std::forward<Args>(args)...) {}

    template <class Elem, class... Args>
        requires std::is_constructible_v<T, std::initializer_list<Elem>&, Args>
    constexpr explicit expected(std::in_place_t, std::initializer_list<Elem> _Ilist, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, std::initializer_list<Elem>&, Args...>) // strengthened
        : MyBase(std::in_place, _Ilist, std::forward<Args>(args)...) {}

    template <class T2>
    using _AllowDirectConversion = std::bool_constant<std::conjunction_v<std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, expected>>,
        std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, std::in_place_t>>, std::is_constructible<T, T2>>>;

    template <class T2 = T>
        requires _AllowDirectConversion<T2>::value
    constexpr explicit(!std::is_convertible_v<T2, T>)
        expected(T2&& _Right) noexcept(std::is_nothrow_constructible_v<T, T2>) // strengthened
        : MyBase(std::in_place, std::forward<T2>(_Right)) {}

    template <class T2>
    struct _AllowUnwrapping : std::bool_constant<!std::disjunction_v<std::is_same<T, T2>, std::is_constructible<T, expected<T2, E>&>,
                                  std::is_constructible<T, const expected<T2, E>&>,
                                  std::is_constructible<T, const expected<T2, E>>, std::is_constructible<T, expected<T2, E>>,
                                  std::is_convertible<expected<T2, E>&, T>, std::is_convertible<const expected<T2, E>&, T>,
                                  std::is_convertible<const expected<T2, E>, T>, std::is_convertible<expected<T2, E>, T>>> {};

    template <class T2>
        requires std::conjunction_v<_AllowUnwrapping<T2>, std::is_constructible<T, const T2&>>
    constexpr explicit(!std::is_convertible_v<const T2&, T>) expected(const expected<T2, E>& _Right) noexcept(
        std::is_nothrow_constructible_v<T, const T2&>) /* strengthened */ {
        if (_Right) {
            this->_Construct(*_Right);
        }
    }

    template <class T2>
    requires std::conjunction_v<_AllowUnwrapping<T2>, std::is_constructible<T, T2>>
    constexpr explicit(!std::is_convertible_v<T2, T>)
        expected(expected<T2, E>&& _Right) noexcept(std::is_nothrow_constructible_v<T, T2>) /* strengthened */ {
        if (_Right) {
            this->_Construct(std::move(*_Right));
        }
    }
#pragma endregion
    

    constexpr expected& operator=(nullopt_t) noexcept {
        reset();
        return *this;
    }

    template <class T2 = T>
        requires std::conjunction_v<std::negation<std::is_same<expected, _Remove_cvref_t<T2>>>,
                    std::negation<std::conjunction<std::is_scalar<T>, std::is_same<T, std::decay_t<T2>>>>,
                    std::is_constructible<T, T2>, std::is_assignable<T&, T2>>
    constexpr expected& operator=(T2&& _Right) noexcept(
        std::is_nothrow_assignable_v<T&, T2>&& std::is_nothrow_constructible_v<T, T2>) /* strengthened */ {
        this->_Assign(std::forward<T2>(_Right));
        return *this;
    }

    template <class T2>
    struct _AllowUnwrappingAssignment
        : std::bool_constant<!std::disjunction_v<std::is_same<T, T2>, std::is_assignable<T&, expected<T2, E>&>,
              std::is_assignable<T&, const expected<T2,E>&>, std::is_assignable<T&, const expected<T2, E>>,
              std::is_assignable<T&, expected<T2,E>>>> {};

    template <class T2>
        requires std::conjunction_v<_AllowUnwrappingAssignment<T2>,
                    std::is_constructible<T, const T2&>, std::is_assignable<T&, const T2&>>
    constexpr expected& operator=(const expected<T2, E>& _Right) noexcept(
        std::is_nothrow_assignable_v<T&, const T2&>&& std::is_nothrow_constructible_v<T, const T2&>) /* strengthened */ {
        if (_Right) {
            this->_Assign(*_Right);
        } else {
            reset();
        }
        return *this;
    }

    template <class T2>
        requires std::conjunction_v<_AllowUnwrappingAssignment<T2>, std::is_constructible<T, T2>,
                    std::is_assignable<T&, T2>>
    constexpr expected& operator=(expected<T2, E>&& _Right) noexcept(
        std::is_nothrow_assignable_v<T&, T2>&& std::is_nothrow_constructible_v<T, T2>) /* strengthened */ {
        if (_Right) {
            this->_Assign(std::move(*_Right));
        } else {
            reset();
        }

        return *this;
    }

    template <class... Args>
    constexpr T& emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>) /* strengthened */ {
        reset();
        return this->_Construct(std::forward<Args>(args)...);
    }

    template <class Elem, class... Args>
        requires std::is_constructible_v<T, std::initializer_list<Elem>&, Args...>
    constexpr T& emplace(std::initializer_list<Elem> _Ilist, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, std::initializer_list<Elem>&, Args...>) /* strengthened */ {
        reset();
        return this->_Construct(_Ilist, std::forward<Args>(args)...);
    }

    constexpr void swap(expected& _Right) noexcept(
        std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_swappable_v<T>) {
        static_assert(std::is_move_constructible_v<T>,
            "optional<T>::swap requires T to be move constructible (N4828 [optional.swap]/1).");
        static_assert(!std::is_move_constructible_v<T> || std::is_swappable_v<T>,
            "optional<T>::swap requires T to be swappable (N4828 [optional.swap]/1).");
        if constexpr (detail::is_trivially_swappable_v<T>) {
            using _TrivialBaseTy = detail::expected_destruct_base<T,E>;
            std::swap(static_cast<_TrivialBaseTy&>(*this), static_cast<_TrivialBaseTy&>(_Right));
        } else {
            const bool _Engaged = this->_Has_value;
            if (_Engaged == _Right._Has_value) {
                if (_Engaged) {
                    std::swap(**this, *_Right);
                }
            } else {
                expected& _Source = _Engaged ? *this : _Right;
                expected& _Target = _Engaged ? _Right : *this;
                _Target._Construct(std::move(*_Source));
                _Source.reset();
            }
        }
    }

    [[nodiscard]] constexpr const T* operator->() const noexcept {
        return std::addressof(this->_Value);
    }
    [[nodiscard]] constexpr T* operator->() noexcept {
        return std::addressof(this->_Value);
    }

    using MyBase::operator*;

    constexpr explicit operator bool() const noexcept {
        return this->m_has_value;
    }
    [[nodiscard]] constexpr bool has_value() const noexcept {
        return this->m_has_value;
    }

    [[nodiscard]] constexpr const T& value() const& {
        if (!this->m_has_value) {
            throw gb::bad_expected_access{};
        }

        return this->m_value;
    }
    [[nodiscard]] constexpr T& value() & {
        if (!this->m_has_value) {
            throw gb::bad_expected_access{};
        }

        return this->m_value;
    }
    [[nodiscard]] constexpr T&& value() && {
        if (!this->m_has_value) {
            throw gb::bad_expected_access{};
        }

        return std::move(this->m_value);
    }

    [[nodiscard]] constexpr const T&& value() const&& {
        if (!this->m_has_value) {
            throw gb::bad_expected_access{};
        }

        return std::move(this->m_value);
    }

    template <class T2>
    [[nodiscard]] constexpr std::remove_cv_t<T> value_or(T2&& _Right) const& {
        static_assert(std::is_convertible_v<const T&, std::remove_cv_t<T>>,
            "The const overload of expected<T,void>::value_or requires const T& to be convertible to remove_cv_t<T>.");
        static_assert(std::is_convertible_v<T2, T>,
            "expected<T,void>::value_or(U) requires U to be convertible to T .");

        if (this->m_has_value) {
            return this->m_value;
        }

        return static_cast<std::remove_cv_t<T>>(std::forward<T2>(_Right));
    }
    template <class T2>
    [[nodiscard]] constexpr std::remove_cv_t<T> value_or(T2&& _Right) && {
        static_assert(std::is_convertible_v<T, std::remove_cv_t<T>>,
            "The rvalue overload of expected<T,void>::value_or requires T to be convertible to remove_cv_t<T>.");
        static_assert(std::is_convertible_v<T2, T>,
            "expected<T,void>::value_or(U) requires U to be convertible to T.");

        if (this->m_has_value) {
            return std::move(this->m_value);
        }

        return static_cast<std::remove_cv_t<T>>(std::forward<T2>(_Right));
    }



    using MyBase::reset;


};

template <class T>
expected(T) -> expected<T, void>;

template <class T1, class T2>
[[nodiscard]] constexpr bool operator==(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
    requires requires {
                 { *_Left == *_Right } -> detail::implicitly_convertible_to<bool>;
    }
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left == *_Right;
    }
    return _Left_has_value == _Right_has_value;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool operator!=(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
    requires requires {
                 { *_Left != *_Right } -> detail::implicitly_convertible_to<bool>;
    }
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left != *_Right;
    }
    return _Left_has_value != _Right_has_value;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool operator<(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
    requires requires {
                 { *_Left < *_Right } -> detail::implicitly_convertible_to<bool>;
    }
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left < *_Right;
    }
    return _Left_has_value < _Right_has_value;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool operator>(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
#ifdef __cpp_lib_concepts
    requires requires {
                 { *_Left > *_Right } -> detail::implicitly_convertible_to<bool>;
             }
#endif // __cpp_lib_concepts
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left > *_Right;
    }
    return _Left_has_value > _Right_has_value;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool operator<=(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
    requires requires(const T1& _Elem1, const T2& _Elem2) {
                 { *_Left <= *_Right } -> detail::implicitly_convertible_to<bool>;
             }
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left <= *_Right;
    }
    return _Left_has_value <= _Right_has_value;
}

template <class T1, class T2>
[[nodiscard]] constexpr bool operator>=(const expected<T1, void>& _Left, const expected<T2, void>& _Right)
    requires requires {
                 { *_Left >= *_Right } -> detail::implicitly_convertible_to<bool>;
             }
{
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left >= *_Right;
    }
    return _Left_has_value >= _Right_has_value;
}


template <class T1, std::three_way_comparable_with<T1> T2>
[[nodiscard]] constexpr std::compare_three_way_result_t<T1, T2> operator<=>(const expected<T1, void>& _Left, const expected<T2, void>& _Right) noexcept(noexcept(*_Left <=> *_Right)) /* strengthened */ {
    const bool _Left_has_value  = _Left.has_value();
    const bool _Right_has_value = _Right.has_value();
    if (_Left_has_value && _Right_has_value) {
        return *_Left <=> *_Right;
    }

    return _Left_has_value <=> _Right_has_value;
}

 template <class T>
[[nodiscard]] constexpr bool operator==(const expected<T, void>& _Left, nullopt_t) noexcept {
    return !_Left.has_value();
}


 template <class T>
[[nodiscard]] constexpr std::strong_ordering operator<=>(const expected<T, void>& _Left, nullopt_t) noexcept {
    return _Left.has_value() <=> false;
}


template <class _Ty>
using _Enable_if_bool_convertible = std::enable_if_t<std::is_convertible_v<_Ty, bool>, int>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_equal =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() == std::declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_not_equal =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() != std::declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_less =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() < std::declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_greater =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() > std::declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_less_equal =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() <= std::declval<const _Rhs&>())>;

template <class _Lhs, class _Rhs>
using _Enable_if_comparable_with_greater_equal =
    _Enable_if_bool_convertible<decltype(std::declval<const _Lhs&>() >= std::declval<const _Rhs&>())>;

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator==(const expected<_Ty1, void>& _Left, const _Ty2& _Right){
    if (_Left) {
        return *_Left == _Right;
    }
    return false;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator==(const _Ty1& _Left, const expected<_Ty2, void>& _Right){
    if (_Right) {
        return _Left == *_Right;
    }
    return false;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_not_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator!=(const expected<_Ty1, void>& _Left, const _Ty2& _Right){
    if (_Left) {
        return *_Left != _Right;
    }
    return true;
}
 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_not_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator!=(const _Ty1& _Left, const expected<_Ty2, void>& _Right) {
    if (_Right) {
        return _Left != *_Right;
    }
    return true;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator<(const expected<_Ty1, void>& _Left, const _Ty2& _Right){
    if (_Left) {
        return *_Left < _Right;
    }
    return true;
}
 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator<(const _Ty1& _Left, const expected<_Ty2, void>& _Right){
    if (_Right) {
        return _Left < *_Right;
    }
    return false;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator>(const expected<_Ty1, void>& _Left, const _Ty2& _Right) {
    if (_Left) {
        return *_Left > _Right;
    }
    return false;
}
 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator>(const _Ty1& _Left, const expected<_Ty2, void>& _Right){
    if (_Right) {
        return _Left > *_Right;
    }
    return true;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator<=(const expected<_Ty1, void>& _Left, const _Ty2& _Right) {
    if (_Left) {
        return *_Left <= _Right;
    }
    return true;
}
 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_less_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator<=(const _Ty1& _Left, const expected<_Ty2, void>& _Right){
    if (_Right) {
        return _Left <= *_Right;
    }
    return false;
}

 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator>=(const expected<_Ty1, void>& _Left, const _Ty2& _Right){
    if (_Left) {
        return *_Left >= _Right;
    }
    return false;
}
 template <class _Ty1, class _Ty2, _Enable_if_comparable_with_greater_equal<_Ty1, _Ty2> = 0>
[[nodiscard]] constexpr bool operator>=(const _Ty1& _Left, const expected<_Ty2, void>& _Right) {
    if (_Right) {
        return _Left >= *_Right;
    }
    return true;
}


// clang-format off
template <class T1, class T2>
    requires (!detail::is_specialization_v<T2, expected>)
        && std::three_way_comparable_with<T1, T2>
[[nodiscard]] constexpr std::compare_three_way_result_t<T1, T2>
    operator<=>(const expected<T1, void>& _Left, const T2& _Right)
        noexcept(noexcept(*_Left <=> _Right)) /* strengthened */ {
    // clang-format on
    if (_Left) {
        return *_Left <=> _Right;
    }

    return std::strong_ordering::less;
}


template <class T>
requires std::is_move_constructible_v<T> && std::is_swappable_v<T>
constexpr void swap(expected<T, void>& _Left, expected<T, void>& _Right) noexcept(noexcept(_Left.swap(_Right))) {
    _Left.swap(_Right);
}

template <class T> // LWG-3627
requires std::is_constructible_v<std::decay_t<T>, T>
[[nodiscard]] constexpr expected<std::decay_t<T>, void> make_expected(T&& _Value) noexcept(
    noexcept(expected<std::decay_t<T>, void>{std::forward<T>(_Value)})) /* strengthened */ {
    return expected<std::decay_t<T>, void>{std::forward<T>(_Value)};
}
template <class T, class... Args>
requires std::is_constructible_v<T, Args...>
[[nodiscard]] constexpr expected<T, void> make_expected(Args&&... args) noexcept(
    noexcept(expected<T, void>{std::in_place, std::forward<Args>(args)...})) /* strengthened */ {
    return expected<T, void>{std::in_place, std::forward<Args>(args)...};
}
template <class T, class Elem, class... Args>
    requires std::is_constructible_v<T, std::initializer_list<Elem>&, Args...>
[[nodiscard]] constexpr expected<T, void> make_optional(std::initializer_list<Elem> _Ilist, Args&&... args) noexcept(
    noexcept(expected<T, void>{std::in_place, _Ilist, std::forward<Args>(args)...})) /* strengthened */ {
    return expected<T, void>{std::in_place, _Ilist, std::forward<Args>(args)...};
}


}