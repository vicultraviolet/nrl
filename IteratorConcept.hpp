#pragma once

#include "./Traits.hpp"

namespace Nrl {
    template<typename It>
    concept c_InputIterator = c_Readable<It> && c_Incrementable<It> && c_EqualityComparable<It>;

    template<typename It>
    concept c_BidirectionalIterator = c_InputIterator<It> && c_Decrementable<It>;

    template<typename It>
    concept c_RandomAccessIterator =
        c_BidirectionalIterator<It> &&
        c_TotallyOrdered<It> &&
        requires(It it, size_t n) {
            { it += n } -> c_SameAs<It&>;
            { it -= n } -> c_SameAs<It&>;
            { it +  n } -> c_SameAs<It>;
            { it -  n } -> c_SameAs<It>;
            { it - it } -> c_SameAs<iptr>;
        };
} // namespace Nrl
