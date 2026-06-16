#pragma once

#include "./AlignedHeapAllocator.hpp"

namespace Nrl {
    template<typename T>
    class DefaultDeleter {
    public:
        [[nodiscard]] constexpr static DefaultDeleter New(void) { return DefaultDeleter(); }

        void operator()(Ref<T> ptr) const noexcept {
            auto allocator = Allocator<T>::New();
            allocator.destroy(ptr);
            allocator.dealloc(ptr, 1);
        }
    private:
        constexpr DefaultDeleter(void) = default;
    };
} // namespace Nrl
