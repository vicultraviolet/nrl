#pragma once

#include "./AllocateBytes.hpp"

namespace Nrl {
    template<typename T>
    class AlignedHeapAllocator {
    public:
        using Value_t = T;

        [[nodiscard]] static constexpr auto New(void) { return AlignedHeapAllocator(); }

        [[nodiscard]] Ref<T> alloc(size_t length) {
            return (Ref<T>)AllocateBytes(sizeof(T) * length, alignof(T));
        }
        void dealloc(Ref<T> ref, size_t length_) {
            DeallocateBytes((Ref<ubyte>)ref);
        }

        template<typename... Args>
        void construct(Ref<T> ref, Args&&... args) {
            new (ref.ptr()) T(Forward<Args>(args)...);
        }
        template<typename F, typename... Args>
        void construct_with(Ref<T> ref, F&& factory, Args&&... args) {
            new (ref.ptr()) T(factory(Forward<Args>(args)...));
        }
        template<c_HasMake Args>
        void construct_from(Ref<T> ref, Args&& args) {
            new (ref.ptr()) T(Forward<Args>(args).make());
        }

        void destruct(Ref<T> ref) noexcept {
            ref->~T();
        }
    private:
        constexpr AlignedHeapAllocator(void) = default;
    };

    template<typename T>
    using Allocator = AlignedHeapAllocator<T>;
} // namespace Nrl
