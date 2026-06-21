#pragma once

#include "./Traits.hpp"

#if defined(__has_cpp_attribute) && __has_cpp_attribute(msvc::no_unique_address)
    #define NRL_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#elif defined(__has_cpp_attribute) && __has_cpp_attribute(no_unique_address)
    #define NRL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
    #define NRL_NO_UNIQUE_ADDRESS
#endif

inline void* operator new(size_t, void* p) noexcept { return p; }
inline void operator delete(void*, void*) noexcept {}

namespace Nrl {
    struct InPlaceTag { explicit InPlaceTag(void) = default; };
    inline constexpr InPlaceTag k_InPlace{};

    template<typename T>
    RemoveReference_t<T>&& Move(T&& moved) {
        return (RemoveReference_t<T>&&)moved;
    }

    template<typename T>
    [[nodiscard]] constexpr T&& Forward(RemoveReference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    [[nodiscard]] constexpr T&& Forward(RemoveReference_t<T>&& t) noexcept {
        static_assert(!IsLvalueReference_v<T>, "Bad forward!");
        return static_cast<T&&>(t);
    }

    template<typename T, typename U = T>
    [[nodiscard]] T Exchange(T& replaced, U&& new_value) {
        T old = Move(replaced);
        replaced = Forward<U>(new_value);
        return old;
    }

    template<typename T>
    [[nodiscard]] T New(T&& t) { return Forward<T>(t); }

    template<typename T>
    [[nodiscard]] constexpr T Absolute(T x) {
        if (x > 0)
            return x;
        else
            return -x;
    }
} // namespace Nrl
