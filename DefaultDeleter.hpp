#pragma once

#include "./AlignedHeapAllocator.hpp"

namespace Nrl {
    template<typename T>
    class DefaultDeleter {
    public:
        [[nodiscard]] constexpr static DefaultDeleter New(void) { return DefaultDeleter(); }

        void operator()(Ref<T> ref) const noexcept {
            auto allocator = Allocator<T>::New();
            allocator.destroy(ref);
            allocator.dealloc(ref, 1);
        }
    private:
        constexpr DefaultDeleter(void) = default;
    };
} // namespace Nrl
