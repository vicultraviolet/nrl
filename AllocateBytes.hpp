#pragma once

#include "./Debug.hpp"
#include "./AllocateAligned.hpp"
#include "./Ref.hpp"
#include "./Option.hpp"

namespace Nrl {
    [[nodiscard]] inline Ref<ubyte> AllocateBytes(usize size, usize alignment) {
        void* ptr = AllocateAligned(size, alignment);
        if (!ptr)
            NRL_PANIC("Bad alloc!");

        return RefFromPtr((ubyte*)ptr);
    }

    [[nodiscard]] inline Option<Ref<ubyte>> TryAllocateBytes(usize size, usize alignment) noexcept {
        void* ptr = AllocateAligned(size, alignment);
        if (!ptr)
            return None();

        return SomeWith(Ref<ubyte>::FromPtr, (ubyte*)ptr);
    }

    inline void DeallocateBytes(Ref<ubyte> ref) noexcept {
        FreeAligned((void*)ref.ptr());
    }

    inline void TryDeallocateBytes(Option<Ref<ubyte>> ref) noexcept {
        if (ref.is_some())
            FreeAligned((void*)ref.unwrap().ptr());
    }
} // namespace Nrl
