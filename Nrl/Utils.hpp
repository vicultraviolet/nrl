#pragma once

#include "Nrl/Traits.hpp"

inline void* operator new(size_t, void* p) noexcept { return p; }
inline void operator delete(void*, void*) noexcept {}

namespace Nrl {
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
} // namespace Nrl
