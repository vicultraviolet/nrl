#pragma once

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

    template<typename T> struct RemoveReference { using Type = T; };

    template<typename T> struct RemoveReference<T&> { using Type = T; };

    template<typename T> struct RemoveReference<T&&> { using Type = T; };

    template<typename T>
    using RemoveReference_t = typename RemoveReference<T>::Type;

    template<typename T>
    concept c_Optional = requires(T t) {
        { T::_None() } -> c_SameAs<T>;
        { const_cast<const T&>(t)._is_some() } -> c_SameAs<bool>;
    };
} // namespace Nrl
