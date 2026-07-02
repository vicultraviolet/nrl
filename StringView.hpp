#pragma once

#include "./Chars.hpp"

namespace Nrl {
    class String;

    class StringView {
    public:
        using ValueType = utf8rune;
        using Iterator = Utf8::Iterator;
    public:
        template<c_Char T>
        [[nodiscard]] constexpr static StringView New(Span<T> chars) {
            return StringView(chars);
        }

        template<c_Char T>
        [[nodiscard]] constexpr static StringView Of(const T* chars) {
            return New(Chars::Of(chars));
        }

        [[nodiscard]] constexpr static StringView Of(const String& str);

        [[nodiscard]] constexpr utf8rune operator[](usize i) const { return *at(i); }

        [[nodiscard]] constexpr Iterator at(usize i) const { return Iterator(ref()) + i; }
        [[nodiscard]] constexpr Iterator begin(void) const { return ref(); }
        [[nodiscard]] constexpr Iterator end(void) const { return ref() + size(); }

        [[nodiscard]] constexpr usize get_length(void) const { return Utf8::GetStringLength(m_Chars); }
        [[nodiscard]] constexpr usize size(void) const { return m_Chars.length(); }
        [[nodiscard]] constexpr bool is_empty(void) const { return size() == 0; }

        [[nodiscard]] constexpr bool is_ascii(void) const { return Utf8::IsCharsAscii(m_Chars); }

        [[nodiscard]] constexpr Span<const utf8char> chars(void) const { return m_Chars; }
        [[nodiscard]] constexpr Ref<const utf8char> ref(void) const { return m_Chars.ref(); }

        [[nodiscard]] static StringView _None(void) { return Span<const utf8char>::_None(); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Chars._is_some(); }

        [[nodiscard]] constexpr bool starts_with(StringView _match) const {
            auto in = SubrangeOf(m_Chars);
            auto match = SubrangeOf(_match.m_Chars);
            return in.starts_with(match);
        }
        [[nodiscard]] constexpr bool ends_with(StringView _match) const {
            auto in = SubrangeOf(m_Chars).reverse();
            auto match = SubrangeOf(_match.m_Chars).reverse();
            return in.starts_with(match);
        }

        [[nodiscard]] constexpr bool starts_with(utf8rune rune) const {
            if (is_empty())
                return false;

            return *begin() == rune;
        }
        [[nodiscard]] constexpr bool ends_with(utf8rune rune) const {
            if (is_empty())
                return false;

            return *end() == rune;
        }

        [[nodiscard]] constexpr bool contains(StringView choosing) const { return find(choosing).is_some(); }
        [[nodiscard]] constexpr bool contains(utf8rune rune) const { return find(rune).is_some(); }

        [[nodiscard]] constexpr StringView sub(iptr begin, iptr end) const {
            return New(m_Chars.sub(begin, end));
        }

        [[nodiscard]] constexpr StringView substr(iptr begin, iptr end) const {
            Iterator first = at((usize)begin);
            Iterator last  = first + (usize)(get_length() - begin - end);

            return New(NewSpan(first.ref(), (usize)(last.ref() - first.ref())));
        }

        template<typename F>
        constexpr void split(StringView delimiter, F&& callback) const {
            if (delimiter.is_empty()) {
                callback(*this);
                return;
            }

            const utf8char* ptr = ref().ptr();
            usize start = 0;
            usize limit = size() < delimiter.size() ? 0 : size() - delimiter.size();

            for (usize i = 0; i <= limit; ) {
                bool match = true;
                for (usize j = 0; j < delimiter.size(); j++) {
                    if (ptr[i + j] != delimiter.ref().ptr()[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    callback(sub(start, i - start));
                    i += delimiter.size();
                    start = i;
                } else {
                    i++;
                }
            }
            if (start <= size())
                callback(sub(start, 0));
        }

        template<typename F>
        constexpr void split(utf8rune delimiter, F&& callback) const {
            auto encoded = Utf8::Encode(delimiter);
            auto view = New(SpanOf(encoded));
            split(view, Forward<F>(callback));
        }

        [[nodiscard]] constexpr StringView trim_start(void) const {
            if (is_empty())
                return *this;

            usize i = 0;
            for (utf8char c : m_Chars) {
                if (!Chars::IsTrimmable(c))
                    break;

                i++;
            }

            return sub(i, 0);
        }

        [[nodiscard]] constexpr StringView trim_end(void) const {
            if (is_empty())
                return *this;

            usize i = 0;
            for (auto it = m_Chars.rbegin(); it != m_Chars.rend(); it++) {
                if (!Chars::IsTrimmable(*it))
                    break;

                i++;
            }

            return sub(0, i);
        }

        [[nodiscard]] constexpr StringView trim(void) const { return trim_start().trim_end(); }

        [[nodiscard]] constexpr Option<Iterator> find(StringView match, usize start = 0) const {
            auto needle = NewSubrange(at(start), end());
            auto haystack = NewSubrange(match.begin(), match.end());
            return needle.find(haystack);
        }

        [[nodiscard]] constexpr Option<Iterator> find(utf8rune rune, usize start = 0) const {
            return Chars::FindRune(
                NewSubrange(at(start), end()),
                rune
            );
        }
    private:
        template<c_Char T>
        StringView(Span<T> chars) : m_Chars((Span<const utf8char>)chars) {}
    private:
        Span<const utf8char> m_Chars;
    };

    [[nodiscard]] constexpr bool operator==(StringView lhs, StringView rhs) {
        if (lhs.size() != rhs.size())
            return false;

        auto it1 = lhs.begin();
        auto it2 = rhs.begin();

        while (it1 == lhs.end()) {
            if (*it1 != *it2)
                return false;

            it1++;
            it2++;
        }

        return true;
    }

    [[nodiscard]] constexpr bool operator!=(StringView lhs, StringView rhs) { return !(lhs == rhs); }

    template<c_Char T>
    [[nodiscard]] constexpr bool operator==(StringView lhs, const T* rhs) { return lhs == StringView::Of(rhs); }
    template<c_Char T>
    [[nodiscard]] constexpr bool operator==(const T* lhs, StringView rhs) { return StringView::Of(lhs) == rhs; }
    template<c_Char T>
    [[nodiscard]] constexpr bool operator!=(StringView lhs, const T* rhs) { return !(lhs == rhs); }
    template<c_Char T>
    [[nodiscard]] constexpr bool operator!=(const T* lhs, StringView rhs) { return !(lhs == rhs); }

    namespace StringLiterals {
        [[nodiscard]] constexpr StringView operator""sv(const char* chars, usize size) {
            return StringView::New(NewSpan(RefFromPtr(chars), size));
        }

        [[nodiscard]] constexpr StringView operator""sv(const utf8char* chars, usize size) {
            return StringView::New(NewSpan(RefFromPtr(chars), size));
        }
    } // namespace StringLiterals
    using namespace StringLiterals;
} // namespace Nrl
