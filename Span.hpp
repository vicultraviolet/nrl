#pragma once

#include "./Ref.hpp"
#include "./ArrayIterator.hpp"

namespace Nrl {
    template<typename T>
    concept c_SpanableContainer = requires(T t) {
        typename T::ValueType;
        { t.ref() } -> c_SameAs<Ref<typename T::ValueType>>;
        { t[0] } -> c_SameAs<typename T::ValueType&>;
        { t.length() } -> c_SameAs<size_t>;
    };

    template<typename T>
    class Span {
    public:
        using ValueType = T;
        using Iterator = ArrayIterator<Span>;
        using ReverseIterator = ArrayReverseIterator<Span>;
    public:
        [[nodiscard]] constexpr static Span New(Ref<T> ref, size_t length) {
            return Span(ref, length);
        }

        template<c_SpanableContainer Container>
        [[nodiscard]] constexpr static Span Of(Container& c) {
            return Span(c.ref(), c.length());
        }

        template<c_SpanableContainer Container>
            requires SameAs_v<T, const T>
        [[nodiscard]] constexpr static Span Of(const Container& c) {
            return Span(c.ref(), c.length());
        }

        ~Span(void) {
            m_Length = 0;
        }

        Span(const Span& other) : m_Ref(other.m_Ref), m_Length(other.m_Length) {}
        Span& operator=(const Span& other) {
            if (this == &other)
                return *this;

            m_Ref = other.m_Ref;
            m_Length = other.m_Length;

            return *this;
        }

        Span(Span&& other) : m_Ref(Move(other.m_Ref)), m_Length(Exchange(other.m_Length, 0)) {}
        Span& operator=(Span&& other) {
            if (this == &other)
                return *this;

            m_Ref = Move(other.m_Ref);
            m_Length = Exchange(other.m_Length, 0);

            return *this;
        }

        [[nodiscard]] constexpr Iterator at(size_t i) const { return ref() + i; }
        [[nodiscard]] constexpr Iterator begin(void) const { return at(0); }
        [[nodiscard]] constexpr Iterator end(void) const { return at(m_Length); }

        [[nodiscard]] constexpr ReverseIterator rat(size_t i) const { return at(m_Length - 1).reverse() + i; }
        [[nodiscard]] constexpr ReverseIterator rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr ReverseIterator rend(void) const { return rat(m_Length); }

        [[nodiscard]] constexpr T& operator[](size_t i) const { return *(m_Ref + i); }

        [[nodiscard]] constexpr Ref<T> ref(void) const { return m_Ref; }
        [[nodiscard]] constexpr size_t length(void) const { return m_Length; }

  		template<typename U>
		    requires IsConvertible_v<T*, U*>
		[[nodiscard]] operator Span<U>(void) { return Span<U>::New(m_Ref, m_Length); }

		template<typename U>
		[[nodiscard]] explicit operator Span<U>(void) { return Span<U>::New((Ref<U>)m_Ref, m_Length); }

        [[nodiscard]] static Span _None(void) { return Span(); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some() && m_Length != 0; }
    private:
        Span(void) : m_Ref(Ref<T>::_None()), m_Length(0) {}
        Span(Ref<T> ref, size_t length) : m_Ref(ref), m_Length(length) {}
    private:
        Ref<T> m_Ref;
        size_t m_Length;
    };

    template<typename T>
    [[nodiscard]] constexpr Span<T> NewSpan(Ref<T> ref, size_t length) { return Span<T>::New(ref, length); }

    template<c_SpanableContainer T>
    [[nodiscard]] constexpr auto SpanOf(T& container) {
        return Span<typename T::ValueType>::Of(container);
    }

    template<c_SpanableContainer T>
    [[nodiscard]] constexpr auto SpanOf(const T& container) {
        return Span<const typename T::ValueType>::Of(container);
    }

    template<c_SpanableContainer T>
    [[nodiscard]] constexpr auto ConstSpanOf(const T& container) {
        return SpanOf(container);
    }
} // namespace Nrl
