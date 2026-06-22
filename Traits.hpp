#pragma once

#include "./Primitives.hpp"

namespace Nrl {
    template<typename T, T v>
    struct IntegralConstant {
        static constexpr T value = v;
        using Value = T;
        using Type = IntegralConstant;

        constexpr operator Value() const noexcept { return value; }
        constexpr Value operator()() const noexcept { return value; }
    };

    using TrueType = IntegralConstant<bool, true>;
    using FalseType = IntegralConstant<bool, false>;

    template<typename T, typename U>
    struct SameAs: FalseType {};

    template<typename T>
    struct SameAs<T, T>: TrueType {};

    template<typename T, typename U>
    constexpr bool SameAs_v = SameAs<T, U>::value;

    template<typename T, typename U>
    concept c_SameAs = SameAs_v<T, U> && SameAs_v<U, T>;

    template<typename T>
    struct IsLvalueReference: FalseType {};

    template<typename T>
    struct IsLvalueReference<T&> : TrueType {};

    template<typename T>
    constexpr bool IsLvalueReference_v = IsLvalueReference<T>::value;

    template<typename Base, typename Derived>
    struct IsBaseOf : FalseType {};

    template<typename Base, typename Derived>
    requires requires { (Base*)((Derived*)(nullptr)); }
    struct IsBaseOf<Base, Derived> : TrueType {};

    template<typename Base, typename Derived>
    constexpr bool IsBaseOf_v = IsBaseOf<Base, Derived>::value;

    template<typename T>
    [[nodiscard]] T declval(void) noexcept;

    template <typename From, typename To>
    class IsConvertible {
    private:
        static void TestHelper(To);

        template<typename F>
        [[nodiscard]] static auto Test(int) -> decltype(TestHelper(declval<F>()), TrueType{});

        template<typename F>
        [[nodiscard]] static auto Test(...) -> FalseType;
    public:
        static constexpr bool value = decltype(Test<From>(0))::value;
    };

    template<typename From, typename To>
    inline constexpr bool IsConvertible_v = IsConvertible<From, To>::value;

    template<typename From, typename To>
    concept c_IsConvertible = IsConvertible_v<From, To>;

    template<bool B, typename T = void>
    struct EnableIf {};

    template<typename T>
    struct EnableIf<true, T> { using Type = T; };

    template<bool B, typename T = void>
    using EnableIf_t = typename EnableIf<B, T>::Type;

    template<typename T>
    struct RemoveConst {
        using Type = T;
    };
    template<typename T>
    struct RemoveConst<const T> {
        using Type = T;
    };
    template<typename T>
    using RemoveConst_t = typename RemoveConst<T>::Type;

    template<typename T>
    struct RemovePointer {
        using Type = T;
    };
    template<typename T>
    struct RemovePointer<T*> {
        using Type = T;
    };
    template<typename T>
    struct RemovePointer<T* const> {
        using Type = T;
    };
    template<typename T>
    struct RemovePointer<T* volatile> {
        using Type = T;
    };
    template<typename T>
    struct RemovePointer<T* const volatile> {
        using Type = T;
    };
    template<typename T>
    using RemovePointer_t = typename RemovePointer<T>::Type;

    template<typename T>
    struct RemoveReference {
        using Type = T;
    };
    template<typename T>
    struct RemoveReference<T&> {
        using Type = T;
    };
    template<typename T>
    struct RemoveReference<T&&> {
        using Type = T;
    };
    template<typename T>
    using RemoveReference_t = typename RemoveReference<T>::Type;

    template<typename T>
    concept c_Optional = requires(T t) {
        { T::_None() } -> c_SameAs<T>;
        { const_cast<const T&>(t)._is_some() } -> c_SameAs<bool>;
    };

    template<typename F, typename... Args>
    struct InvokeResult {
        using Type = decltype(declval<F>()(declval<Args>()...));
    };

    template<typename F, typename... Args>
    using InvokeResult_t = typename InvokeResult<F, Args...>::Type;

    template<typename T>
    concept c_HasMake = requires(T t) {
        { t.make() };
    };

    template<typename T>
    concept c_Readable = requires(T t) {
        typename T::ValueType;
        { *t } -> c_IsConvertible<typename T::ValueType>;
        //{ t.operator->() } -> c_SameAs<typename T::ValueType*>;
    };

    template<typename T>
    concept c_Incrementable = requires(T t) {
        { ++t } -> c_SameAs<T&>;
        { t++ } -> c_SameAs<T>;
    };

    template<typename T>
    concept c_Decrementable = requires(T t) {
        { --t } -> c_SameAs<T&>;
        { t-- } -> c_SameAs<T>;
    };

    template<typename T>
    concept c_EqualityComparable = requires(T a, T b) {
        { a == b } -> c_SameAs<bool>;
        { a != b } -> c_SameAs<bool>;
    };

    template<typename T>
    concept c_TotallyOrdered = requires(T a, T b) {
        { a <  b } -> c_SameAs<bool>;
        { a >  b } -> c_SameAs<bool>;
        { a <= b } -> c_SameAs<bool>;
        { a >= b } -> c_SameAs<bool>;
    };

    template<typename T>
    concept c_Char =
        SameAs_v<RemoveConst_t<T>, char> ||
        SameAs_v<RemoveConst_t<T>, ubyte> ||
        SameAs_v<RemoveConst_t<T>, utf8char>;
} // namespace Nrl
