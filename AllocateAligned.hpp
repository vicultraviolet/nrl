#pragma once

#include <stdlib.h>
#include "./Primitives.hpp"

namespace Nrl {
    [[nodiscard]] constexpr bool IsPowerOfTwo(size_t x) {
        return x && (x & (x - 1)) == 0;
    }

    [[nodiscard]] constexpr bool IsAligned(void* ptr, size_t alignment) {
        return ((uptr)ptr & (alignment - 1)) == 0;
    }

    [[nodiscard]] inline void* AllocateAligned(size_t size, size_t alignment) {
        if (size == 0)
            return nullptr;

        if (alignment < k_PtrSize)
            alignment = k_PtrSize;

        if (!IsPowerOfTwo(alignment))
            return nullptr;

        size_t required = size + alignment - 1 + k_PtrSize;
        void* raw = malloc(required);
        if (!raw)
            return nullptr;

        uptr address = (uptr)raw;
        uptr aligned_address = (address + k_PtrSize + alignment - 1) & ~(alignment - 1);
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
