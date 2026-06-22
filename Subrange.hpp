#pragma once

#include "./IteratorConcept.hpp"

namespace Nrl {
    template<c_RandomAccessIterator T>
    [[nodiscard]] constexpr iptr Distance(T first, T last) {
        return last - first;
    }

    template<c_InputIterator T>
    [[nodiscard]] constexpr iptr Distance(T first, T last) {
        iptr count = 0;
        while (first != last) {
            first++;
            count++;
        }
        return count;
    }

    template<c_InputIterator T>
    class Subrange {
    public:
        using ValueType = T::ValueType;
        using Iterator = T;
    public:
        [[nodiscard]] static constexpr Subrange New(T&& begin, T&& end) {
            return Subrange(Forward<T>(begin), Forward<T>(end));
        }

        template<typename Container>
            requires SameAs_v<T, typename Container::Iterator>
        [[nodiscard]] static constexpr Subrange Of(Container& c) {
            return Subrange(c.begin(), c.end());
        }

        template<typename Container>
            requires SameAs_v<T, typename Container::ConstIterator>
        [[nodiscard]] static constexpr Subrange Of(const Container& c) {
            return Subrange(c.begin(), c.end());
        }

        [[nodiscard]] constexpr auto reverse(void) const requires c_BidirectionalIterator<T> {
            return Subrange<typename T::Reversed>::New(m_End.reverse() + 1, m_Begin.reverse() + 1);
        }

        [[nodiscard]] constexpr Iterator at(usize i) const { return m_Begin + i; }
        [[nodiscard]] constexpr Iterator begin(void) const { return m_Begin; }
        [[nodiscard]] constexpr Iterator end(void) const { return m_End; }

        [[nodiscard]] constexpr iptr distance(void) const { return Distance(m_Begin, m_End); }
        [[nodiscard]] constexpr usize length(void) const { return (usize)Absolute(distance()); }

  		template<typename U>
		    requires IsConvertible_v<T, U>
		[[nodiscard]] operator Subrange<U>(void) const { return Subrange<U>::New(m_Begin, m_End); }

        [[nodiscard]] static Subrange _None(void) { return New(T::_None(), T::_None()); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Begin != m_End; }
    private:
        Subrange(T&& begin, T&& end) : m_Begin(Forward<T>(begin)), m_End(Forward<T>(end)) {}
    private:
        T m_Begin;
        T m_End;
    };

    template<typename T>
    [[nodiscard]] constexpr Subrange<T> NewSubrange(T&& begin, T&& end) { return Subrange<T>::New(Forward<T>(begin), Forward<T>(end)); }

    template<typename T>
    [[nodiscard]] constexpr auto SubrangeOf(T& container) {
        using Iterator = typename T::Iterator;
        return Subrange<Iterator>::Of(container);
    }

    template<typename T>
    [[nodiscard]] constexpr auto SubrangeOf(const T& container) {
        using Iterator = typename T::ConstIterator;
        return Subrange<Iterator>::Of(container);
    }

    template<typename T>
    [[nodiscard]] constexpr auto ConstSubrangeOf(const T& container) { return SubrangeOf(container); }
} // namespace Nrl
