#pragma once

#include "./Debug.hpp"
#include "./AllocateAligned.hpp"
#include "./Ref.hpp"
#include "./Option.hpp"

namespace Nrl {
    [[nodiscard]] inline Ref<ubyte> AllocateBytes(size_t size, size_t alignment) {
        void* ptr = AllocateAligned(size, alignment);
        if (!ptr)
            NRL_PANIC("Bad alloc!");

        return Ref<ubyte>::New(*(ubyte*)ptr);
    }

    [[nodiscard]] inline Option<Ref<ubyte>> TryAllocateBytes(size_t size, size_t alignment) noexcept {
        void* ptr = AllocateAligned(size, alignment);
        if (!ptr)
            return None();

        return SomeWith<Ref<ubyte>>(Ref<ubyte>::New, *(ubyte*)ptr);
    }

    inline void DeallocateBytes(Ref<ubyte> ref) noexcept {
        FreeAligned((void*)ref.ptr());
    }

    inline void TryDeallocateBytes(Option<Ref<ubyte>> ref) noexcept {
        if (ref.is_some())
            FreeAligned((void*)ref.unwrap().ptr());
    }
} // namespace Nrl
