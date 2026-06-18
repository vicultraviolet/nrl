#pragma once

namespace Nrl {
    template<typename T, T... Ints>
    struct IntegerSequence {
        using ValueType = T;
        static constexpr size_t Length(void) noexcept { return sizeof...(Ints); }
    };

    // Helper that uses a size_t counter (non-dependent type)
    template<typename T, size_t N, T... Ints>
    struct NewIntegerSequenceImpl {
        using Type = typename NewIntegerSequenceImpl<T, N - 1, (T)(N - 1), Ints...>::Type;
    };

    template<typename T, T... Ints>
    struct NewIntegerSequenceImpl<T, 0, Ints...> {
        using Type = IntegerSequence<T, Ints...>;
    };

    template<typename T, T N>
    struct NewIntegerSequenceHelper {
        static_assert(N >= 0, "N must be non-negative");
        using Type = typename NewIntegerSequenceImpl<T, static_cast<size_t>(N)>::Type;
    };

    template<typename T, T N>
    using NewIntegerSequence = typename NewIntegerSequenceHelper<T, N>::Type;

    template<size_t... Ints>
    using IndexSequence = IntegerSequence<size_t, Ints...>;

    template<size_t N>
    using NewIndexSequence = NewIntegerSequence<size_t, N>;

    template<typename T, T... Ints>
    [[nodiscard]] constexpr size_t IntegerSequenceLength(IntegerSequence<T, Ints...>) noexcept {
        return sizeof...(Ints);
    }
} // namespace Nrl
