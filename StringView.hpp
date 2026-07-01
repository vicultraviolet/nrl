#pragma once

#include "./String.hpp"

namespace Nrl {
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

        [[nodiscard]] constexpr static StringView Of(const String& s) {
            return New(s.chars());
        }

        [[nodiscard]] constexpr utf8rune operator[](usize i) const { return *at(i); }

        [[nodiscard]] constexpr Iterator at(usize i) const { return Iterator(ref()) + i; }
        [[nodiscard]] constexpr Iterator begin(void) const { return ref(); }
        [[nodiscard]] constexpr Iterator end(void) const { return ref() + size(); }

        [[nodiscard]] constexpr usize get_length(void) const { return Utf8::GetStringLength(m_Chars); }
        [[nodiscard]] constexpr usize size(void) const { return m_Chars.length(); }

        [[nodiscard]] constexpr bool is_ascii(void) const { return Utf8::IsCharsAscii(m_Chars); }

        [[nodiscard]] constexpr Span<const utf8char> chars(void) const { return m_Chars; }
        [[nodiscard]] constexpr Ref<const utf8char> ref(void) const { return m_Chars.ref(); }

        [[nodiscard]] static StringView _None(void) { return Span<const utf8char>::_None(); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Chars._is_some(); }
    private:
        template<c_Char T>
        StringView(Span<T> chars) : m_Chars((Span<const utf8char>)chars) {}
    private:
        Span<const utf8char> m_Chars;
    };

    [[nodiscard]] constexpr StringView operator""sv(const char* chars, usize size) {
        return StringView::New(NewSpan(RefFromPtr(chars), size));
    }

    [[nodiscard]] constexpr StringView operator""sv(const utf8char* chars, usize size) {
        return StringView::New(NewSpan(RefFromPtr(chars), size));
    }

    [[nodiscard]] constexpr String StringFrom(StringView view) { return String::New(view.chars()); }
} // namespace Nrl
