#pragma once

#include "./IteratorConcept.hpp"
#include "./Option.hpp"

namespace Nrl {
    template<c_RandomAccessIterator It>
    [[nodiscard]] constexpr isize Distance(It first, It last) {
        return last - first;
    }

    template<c_InputIterator It>
    [[nodiscard]] constexpr isize Distance(It first, It last) {
        isize count = 0;
        while (first != last) {
            first++;
            count++;
        }
        return count;
    }

    template<c_InputIterator It>
    class Subrange {
    public:
        using ValueType = It::ValueType;
        using Iterator = It;
    public:
        [[nodiscard]] static constexpr Subrange New(It&& begin, It&& end) {
            return Subrange(Forward<It>(begin), Forward<It>(end));
        }

        template<typename Container>
            requires SameAs_v<It, typename Container::Iterator>
        [[nodiscard]] static constexpr Subrange Of(Container& c) {
            return Subrange(c.begin(), c.end());
        }

        template<typename Container>
            requires SameAs_v<It, typename Container::ConstIterator>
        [[nodiscard]] static constexpr Subrange Of(const Container& c) {
            return Subrange(c.begin(), c.end());
        }

        [[nodiscard]] constexpr auto reverse(void) const requires c_BidirectionalIterator<It> {
            return Subrange<typename It::Reversed>::New(m_End.reverse() + 1, m_Begin.reverse() + 1);
        }

        [[nodiscard]] constexpr bool starts_with(Subrange<It> match) {
            if (match.length() > length())
                return false;

            auto it1 = match.begin();
            auto it2 = begin();
            while (it1 != match.end()) {
                if (*it1 != *it2)
                    return false;

                it1++;
                it2++;
            }

            return true;
        }

        [[nodiscard]] constexpr Option<It> find(Subrange<It> needle) {
            for (auto it1 = m_Begin; it1 != m_End; it1++) {
                auto it2 = needle.m_Begin;

                while (it2 != needle.m_End && it1 != m_End && *it1 == *it2) {
                    it1++;
                    it2++;
                }

                if (it2 == needle.m_End)
                    return Some(it1);
            }

            return None();
        }
        [[nodiscard]] constexpr bool contains(Subrange<It> needle) { return find().is_some(); }

        [[nodiscard]] constexpr Iterator at(usize i) const { return m_Begin + i; }
        [[nodiscard]] constexpr Iterator begin(void) const { return m_Begin; }
        [[nodiscard]] constexpr Iterator end(void) const { return m_End; }

        [[nodiscard]] constexpr isize distance(void) const { return Distance(m_Begin, m_End); }
        [[nodiscard]] constexpr usize length(void) const { return (usize)Absolute(distance()); }
        [[nodiscard]] constexpr bool is_empty(void) const { return length() == 0; }

  		template<typename It2>
		    requires IsConvertible_v<It, It2>
		[[nodiscard]] operator Subrange<It2>(void) const { return Subrange<It2>::New(m_Begin, m_End); }

        [[nodiscard]] static Subrange _None(void) { return New(It::_None(), It::_None()); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Begin._is_some() && m_End._is_some(); }
    private:
        Subrange(It&& begin, It&& end) : m_Begin(Forward<It>(begin)), m_End(Forward<It>(end)) {}
    private:
        It m_Begin;
        It m_End;
    };

    template<typename It>
    [[nodiscard]] constexpr Subrange<It> NewSubrange(It&& begin, It&& end) { return Subrange<It>::New(Forward<It>(begin), Forward<It>(end)); }

    template<typename T>
    [[nodiscard]] constexpr auto SubrangeOf(T& container) {
        using It = decltype(container.begin());
        return Subrange<It>::Of(container);
    }

    template<typename T>
    [[nodiscard]] constexpr auto SubrangeOf(const T& container) {
        using It = decltype(container.begin());
        return Subrange<It>::Of(container);
    }

    template<typename T>
    [[nodiscard]] constexpr auto ConstSubrangeOf(const T& container) { return SubrangeOf(container); }


} // namespace Nrl
