#pragma once
#include <type_traits>
#include <optional>
#include "unexpected.h"

namespace gb{

namespace detail
{
    struct nontrivial_dummy_type {
        constexpr nontrivial_dummy_type() noexcept {} //has to be user defined (not default)
    };

    template <class _From, class _To>
    concept implicitly_convertible_to = std::is_convertible_v<_From, _To>;

    template <class _Type, template <class...> class _Template>
    inline constexpr bool is_specialization_v = false; // true if and only if _Type is a specialization of _Template
    template <template <class...> class _Template, class... _Types>
    inline constexpr bool is_specialization_v<_Template<_Types...>, _Template> = true;



    template<class T, class... Args>
    constexpr void construct_in_place(T& obj, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        if (std::is_constant_evaluated()) 
        {
            std::construct_at(std::addressof(obj), std::forward<Args>(args)...);
        } 
        else
        {
            //call new on the location of this->m_value
            if constexpr (std::is_pointer_v<T>) 
            {
                new (const_cast<void*>(static_cast<const volatile void*>(obj))) T(std::forward<Args>(args)...);
            } else 
            {
                new (const_cast<void*>(static_cast<const volatile void*>(std::addressof(*obj)))) T(std::forward<Args>(args)...);
            }
        }
    }

    template <class, class = void>
    struct has_ADL_swap : std::false_type {};
    template <class T>
    struct has_ADL_swap<T, std::void_t<decltype(std::swap(std::declval<T&>(), std:: declval<T&>()))>> : std::true_type {};

    template <class T>
    inline constexpr bool is_trivially_swappable_v = std::conjunction_v<std::is_trivially_destructible<T>,
    std::is_trivially_move_constructible<T>, std::is_trivially_move_assignable<T>, std::negation<has_ADL_swap<T>>>;


#pragma region SMF CONTROL

    template <class Base>
    struct not_trivially_copiable : Base { // non-trivial copy construction facade
        using Base::Base;

        not_trivially_copiable() = default;
        constexpr not_trivially_copiable(const not_trivially_copiable& _That) noexcept(
            noexcept(Base::_Construct_from(static_cast<const Base&>(_That)))) {
            Base::_Construct_from(static_cast<const Base&>(_That));
        }
        not_trivially_copiable(not_trivially_copiable&&)                 = default;
        not_trivially_copiable& operator=(const not_trivially_copiable&) = default;
        not_trivially_copiable& operator=(not_trivially_copiable&&)      = default;
    };

    template <class base, class... Args>
    using control_copy = std::conditional_t<
        std::conjunction_v<std::is_copy_constructible<Args>..., std::negation<std::conjunction<std::is_trivially_copy_constructible<Args>...>>>,
        not_trivially_copiable<base>, base>;


    template <class Base, class... Args>
    struct not_trivially_movable : control_copy<Base, Args...> { // non-trivial move construction facade
        using MyBase = control_copy<Base, Args...>;
        using MyBase::MyBase;

        not_trivially_movable()                         = default;
        not_trivially_movable(const not_trivially_movable&) = default;
        constexpr not_trivially_movable(not_trivially_movable&& _That) noexcept(
            noexcept(MyBase::_Construct_from(static_cast<Base&&>(_That)))) {
            MyBase::_Construct_from(static_cast<Base&&>(_That));
        }
        not_trivially_movable& operator=(const not_trivially_movable&) = default;
        not_trivially_movable& operator=(not_trivially_movable&&)      = default;
    };

    template <class Base, class... Args>
    using control_move = std::conditional_t<
        std::conjunction_v<std::is_move_constructible<Args>..., std::negation<std::conjunction<std::is_trivially_move_constructible<Args>...>>>,
        not_trivially_movable<Base, Args...>, control_copy<Base, Args...>>;


    template <class Base, class... Args>
    struct not_trivially_copy_assignable : control_move<Base, Args...> { // non-trivial copy assignment facade
        using Mybase = control_move<Base, Args...>;
        using Mybase::Mybase;

        not_trivially_copy_assignable()                                = default;
        not_trivially_copy_assignable(const not_trivially_copy_assignable&) = default;
        not_trivially_copy_assignable(not_trivially_copy_assignable&&)      = default;

        constexpr not_trivially_copy_assignable& operator=(const not_trivially_copy_assignable& _That) noexcept(
            noexcept(Mybase::_Assign_from(static_cast<const Base&>(_That)))) {
            Mybase::_Assign_from(static_cast<const Base&>(_That));
            return *this;
        }
        not_trivially_copy_assignable& operator=(not_trivially_copy_assignable&&) = default;
    };

    template <class Base, class... Args>
    struct deleted_copy_assignment : control_move<Base, Args...> { // deleted copy assignment facade
        using Mybase = control_move<Base, Args...>;
        using Mybase::Mybase;

        deleted_copy_assignment()                                          = default;
        deleted_copy_assignment(const deleted_copy_assignment&)            = default;
        deleted_copy_assignment(deleted_copy_assignment&&)                 = default;
        deleted_copy_assignment& operator=(const deleted_copy_assignment&) = delete;
        deleted_copy_assignment& operator=(deleted_copy_assignment&&)      = default;
    };

    template <class Base, class... Args>
    using control_copy_assign =
        std::conditional_t<std::conjunction_v<std::is_trivially_destructible<Args>..., std::is_trivially_copy_constructible<Args>...,
                        std::is_trivially_copy_assignable<Args>...>,
            control_move<Base, Args...>,
            std::conditional_t<std::conjunction_v<std::is_copy_constructible<Args>..., std::is_copy_assignable<Args>...>,
                not_trivially_copy_assignable<Base, Args...>, deleted_copy_assignment<Base, Args...>>>;


    template <class Base, class... Args>
    struct not_trivally_move_assignable : control_copy_assign<Base, Args...> { // non-trivial move assignment facade
        using MyBase = control_copy_assign<Base, Args...>;
        using MyBase::MyBase;

        not_trivally_move_assignable()                                               = default;
        not_trivally_move_assignable(const not_trivally_move_assignable&)            = default;
        not_trivally_move_assignable(not_trivally_move_assignable&&)                 = default;
        not_trivally_move_assignable& operator=(const not_trivally_move_assignable&) = default;

        constexpr not_trivally_move_assignable& operator=(not_trivally_move_assignable&& _That) noexcept(
            noexcept(MyBase::_Assign_from(static_cast<Base&&>(_That)))) {
            MyBase::_Assign_from(static_cast<Base&&>(_That));
            return *this;
        }
    };

    template <class Base, class... Args>
    struct deleted_move_assignment : control_copy_assign<Base, Args...> { // deleted move assignment facade
        using MyBase = control_copy_assign<Base, Args...>;
        using MyBase::MyBase;

        deleted_move_assignment()                                          = default;
        deleted_move_assignment(const deleted_move_assignment&)            = default;
        deleted_move_assignment(deleted_move_assignment&&)                 = default;
        deleted_move_assignment& operator=(const deleted_move_assignment&) = default;
        deleted_move_assignment& operator=(deleted_move_assignment&&)      = delete;
    };

    template <class Base, class... Args>
    using control_move_assign =
        std::conditional_t<std::conjunction_v<std::is_trivially_destructible<Args>..., std::is_trivially_move_constructible<Args>...,
                        std::is_trivially_move_assignable<Args>...>,
            control_copy_assign<Base, Args...>,
            std::conditional_t<std::conjunction_v<std::is_move_constructible<Args>..., std::is_move_assignable<Args>...>,
                not_trivally_move_assignable<Base, Args...>, deleted_move_assignment<Base, Args...>>>;

    template <class Base, class... Args>
    using control = control_move_assign<Base, Args...>;
#pragma endregion

template<class T, class E>
struct expected_destruct_base{};

template<class T, class E>
    requires std::is_void_v<E> && std::is_trivially_destructible_v<T>
struct expected_destruct_base<T,E>
{
    union {
        nontrivial_dummy_type m_dummy;
        std::remove_const_t<T> m_value;
    };

    bool m_has_value;

    constexpr expected_destruct_base() noexcept : m_dummy{}, m_has_value{false} {} //initialize an empty expected

    template <class... Args>
    constexpr explicit expected_destruct_base(std::in_place_t, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_value(std::forward<Args>(args)...), m_has_value{true} {} // initialize contained value with args...

    constexpr void reset() noexcept {
        m_has_value = false;
    }
};

template<class T, class E>
    requires std::is_void_v<E> && (!std::is_trivially_destructible_v<T>)
struct expected_destruct_base<T,E>
{
    union {
        nontrivial_dummy_type m_dummy;
        std::remove_const_t<T> m_value;
    };

    bool m_has_value;

    constexpr expected_destruct_base() noexcept : m_dummy{}, m_has_value{false} {} //initialize an empty expected

    template <class... Args>
    constexpr explicit expected_destruct_base(std::in_place_t, Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
        : m_value(std::forward<Args>(args)...), m_has_value{true} {} // initialize contained value with args...

    expected_destruct_base(const expected_destruct_base&)            = default;
    expected_destruct_base(expected_destruct_base&&)                 = default;
    expected_destruct_base& operator=(const expected_destruct_base&) = default;
    expected_destruct_base& operator=(expected_destruct_base&&)      = default;

    constexpr void reset() noexcept {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }
};

template<class T, class E>
struct expected_construct_base : expected_destruct_base<T, E>
{};

template<class T, class E>
    requires std::is_void_v<E> && (!std::is_void_v<T>)
struct expected_construct_base<T, E> : expected_destruct_base<T,E>
{
    using expected_destruct_base<T,E>::expected_destruct_base;

    template <class... Args>
    constexpr T& _Construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
        // transition from the empty to the value-containing state
        construct_in_place(this->m_value, args);
        this->_Has_value = true;
        return this->_Value;
    }

    template <class T2>
    constexpr void _Assign(T2&& _Right) noexcept(
        std::is_nothrow_assignable_v<T&, T2>&& std::is_nothrow_constructible_v<T, T2>) {
        // assign / initialize the contained value from _Right
        if (this->m_has_value) {
            this->m_value = std::forward<T2>(_Right);
        } else {
            _Construct(std::forward<T2>(_Right));
        }
    }

    template <class Self>
    constexpr void _Construct_from(Self&& _Right) noexcept(std::is_nothrow_constructible_v<T, decltype(*std::forward<Self>(_Right))>) {
        // initialize contained value from _Right iff it contains a value
        if (_Right.m_has_value) {
            _Construct(*std::forward<Self>(_Right));
        }
    }

    template <class Self>
    constexpr void _Assign_from(Self&& _Right) noexcept(
        std::is_nothrow_constructible_v<T, decltype(*std::forward<Self>(_Right))>&&
            std::is_nothrow_assignable_v<T&, decltype(*std::forward<Self>(_Right))>) {
        // assign/initialize/destroy contained value from _Right
        if (_Right.m_has_value) {
            _Assign(*std::forward<Self>(_Right));
        } else {
            this->reset();
        }
    }

    [[nodiscard]] constexpr T& operator*() & noexcept {
        return this->_Value;
    }

    [[nodiscard]] constexpr const T& operator*() const& noexcept {
        return this->_Value;
    }

    [[nodiscard]] constexpr T&& operator*() && noexcept {
        return std::move(this->_Value);
    }

    [[nodiscard]] constexpr const T&& operator*() const&& noexcept {
        return std::move(this->_Value);
    }
};

}

class bad_expected_access : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return "Bad expected access";
    }
};

template<class T, class E>
struct expected
{
    static_assert(!(std::is_same_v<std::remove_cv_t<T>, nullopt_t> || std::is_same_v<std::remove_cv_t<T>, std::in_place_t>),
        "T in expected<T> must be a type other than nullopt_t or in_place_t.");
    static_assert(std::is_object_v<T> && std::is_destructible_v<T> && !std::is_array_v<T>,
        "T in expected<T> must meet the Cpp17Destructible requirements.");

    using value_type = T;
    using error_type = E;
};

}