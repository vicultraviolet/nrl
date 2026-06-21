#pragma once

#include <stdlib.h>
#include "./Primitives.hpp"

namespace Nrl {
    [[nodiscard]] constexpr bool IsPowerOfTwo(usize x) {
        return x && (x & (x - 1)) == 0;
    }

    [[nodiscard]] constexpr bool IsAligned(void* ptr, usize alignment) {
        return ((uptr)ptr & (alignment - 1)) == 0;
    }

    [[nodiscard]] inline void* AllocateAligned(usize size, usize alignment) {
        if (size == 0)
            return nullptr;

        if (alignment < ptrsize)
            alignment = ptrsize;

        if (!IsPowerOfTwo(alignment))
            return nullptr;

        usize required = size + alignment - 1 + ptrsize;
        void* raw = malloc(required);
        if (!raw)
            return nullptr;

        uptr address = (uptr)raw;
        uptr aligned_address = (address + ptrsize + alignment - 1) & ~(alignment - 1);
        void* ptr = (void*)aligned_address;

        ((void**)ptr)[-1] = raw;
        return ptr;
    }

    inline void FreeAligned(void* ptr) noexcept {
        if (!ptr)
            return;

        void* raw = ((void**)ptr)[-1];
        free(raw);
    }
} // namespace Nrl
