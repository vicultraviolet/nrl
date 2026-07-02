#pragma once

#include <string.h>
#include <stdio.h>

#include "./Span.hpp"
#include "./Subrange.hpp"
#include "./Pool.hpp"

namespace Nrl {
    namespace Utf8 {
        template<c_Char T>
        [[nodiscard]] constexpr usize GetCharLength(T _c) {
            auto c = (ubyte)_c;

            if ((c & 0x80) == 0)
                return 1;
            if ((c & 0xE0) == 0xC0)
                return 2;
            if ((c & 0xF0) == 0xE0)
                return 3;
            if ((c & 0xF8) == 0xF0)
                return 4;

            return 0;
        }

        template<c_Char T>
        [[nodiscard]] constexpr bool IsCharsAscii(Span<const T> chars) {
            for (auto c : chars) {
                if (GetCharLength(c) != 1)
                    return false;
            }
            return true;
        }

        template<c_Char T>
        [[nodiscard]] constexpr usize GetStringLength(Span<const T> chars) {
            usize length = 0;

            for (usize i = 0; i < chars.length(); i += GetCharLength(chars[i]))
                length++;

            return length;
        }

        template<c_Char T>
        [[nodiscard]] constexpr usize GetStringSize(Ref<const T> chars) {
            return strlen((const char*)chars.ptr());
        }

        [[nodiscard]] constexpr Pool<ubyte, 4> Encode(utf8rune rune) {
            auto bytes = Pool<ubyte, 4>::Empty();

            if (rune < 0x80) {
                bytes.emplace(rune);
            } else
            if (rune < 0x800) {
                bytes.emplace(0xC0 | (rune >> 6));
                bytes.emplace(0x80 | (rune & 0x3F));
            } else
            if (rune < 0x10000) {
                bytes.emplace(0xE0 | (rune >> 12));
                bytes.emplace(0x80 | ((rune >> 6) & 0x3F));
                bytes.emplace(0x80 | (rune & 0x3F));
            } else
            if (rune < 0x110000) {
                bytes.emplace(0xF0 | (rune >> 18));
                bytes.emplace(0x80 | ((rune >> 12) & 0x3F));
                bytes.emplace(0x80 | ((rune >> 6) & 0x3F));
                bytes.emplace(0x80 | (rune & 0x3F));
            }

            return bytes;
        }

        [[nodiscard]] constexpr utf8rune Decode(Span<const ubyte> ch) {
            switch (ch.length()) {
                case 1: // ascii
                    return (utf8rune)(ch[0]);
                case 2:
                    return
                        (utf8rune)(ch[0] & 0x1F) << 6 |
                        (utf8rune)(ch[1] & 0x3F);
                case 3:
                    return
                        (utf8rune)(ch[0] & 0x0F) << 12 |
                        (utf8rune)(ch[1] & 0x3F) << 6  |
                        (utf8rune)(ch[2] & 0x3F);
                case 4:
                    return
                        (utf8rune)(ch[0] & 0x07) << 18 |
                        (utf8rune)(ch[1] & 0x3F) << 12 |
                        (utf8rune)(ch[2] & 0x3F) << 6  |
                        (utf8rune)(ch[3] & 0x3F);
                default:
                    return 0xFFFD;
            }
        }

        void PrintChars(Span<const ubyte> c) {
            fwrite(c.ref().ptr(), 1, c.length(), stdout);
        }
        void PrintRune(utf8rune rune) {
            ubyte bytes[4];
            usize length = 0;

            if (rune < 0x80) {
                bytes[0] = (ubyte)rune;
                length = 1;
            } else
            if (rune < 0x800) {
                bytes[0] = (ubyte)(0xC0 | (rune >> 6));
                bytes[1] = (ubyte)(0x80 | (rune & 0x3F));
                length = 2;
            } else
            if (rune < 0x10000) {
                bytes[0] = (ubyte)(0xE0 | (rune >> 12));
                bytes[1] = (ubyte)(0x80 | ((rune >> 6) & 0x3F));
                bytes[2] = (ubyte)(0x80 | (rune & 0x3F));
                length = 3;
            } else
            if (rune < 0x110000) {
                bytes[0] = (ubyte)(0xF0 | (rune >> 18));
                bytes[1] = (ubyte)(0x80 | ((rune >> 12) & 0x3F));
                bytes[2] = (ubyte)(0x80 | ((rune >> 6) & 0x3F));
                bytes[3] = (ubyte)(0x80 | (rune & 0x3F));
                length = 4;
            } else {
                bytes[0] = 0xEF;
                bytes[1] = 0xBF;
                bytes[2] = 0xBD;
                length = 3;
            }

            fwrite(bytes, 1, length, stdout);
        }

        class Iterator {
    	public:
    	    using T = const utf8char;
    	    using ValueType = utf8rune;

            Iterator(Ref<T> ref) : m_Ref(ref) {}

    		constexpr auto& operator++(void) {
    		    m_Ref += current_char_length();
                return *this;
    		}
    		constexpr auto operator++(int) {
    		    auto temp = *this;
    			m_Ref += current_char_length();
                   	return temp;
    		}

            constexpr auto operator+(usize x) const {
                auto it = *this;
                for (usize i = 0; i < x; i++)
                    it++;
                return it;
            }
            constexpr auto& operator+=(usize x) {
                *this = *this + x;
                return *this;
            }

    		[[nodiscard]] constexpr ValueType operator*(void) const {
    		    return Utf8::Decode(NewSpan((Ref<ubyte>)m_Ref, current_char_length()));
    		}

    		[[nodiscard]] constexpr Ref<T> ref(void) const { return m_Ref; }
    		[[nodiscard]] constexpr T* ptr(void) const { return m_Ref.ptr(); }

    		//[[nodiscard]] constexpr Reversed reverse(void) const { return m_Ref; }

    		[[nodiscard]] constexpr bool operator==(Iterator other) const { return m_Ref == other.m_Ref; }
    		[[nodiscard]] constexpr bool operator!=(Iterator other) const { return m_Ref != other.m_Ref; }
    		[[nodiscard]] constexpr bool operator> (Iterator other) const { return m_Ref >  other.m_Ref; }
    		[[nodiscard]] constexpr bool operator< (Iterator other) const { return m_Ref <  other.m_Ref; }
    		[[nodiscard]] constexpr bool operator>=(Iterator other) const { return m_Ref >= other.m_Ref; }
    		[[nodiscard]] constexpr bool operator<=(Iterator other) const { return m_Ref <= other.m_Ref; }

    		[[nodiscard]] usize current_char_length(void) const { return Utf8::GetCharLength(*m_Ref); }

    		[[nodiscard]] static Iterator _None(void) { return Ref<T>::_None(); }
    		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
    	private:
    	    Ref<T> m_Ref;
        };

        void Print(Subrange<Iterator> str) {
            for (auto it = str.begin(); it != str.end(); it++)
                PrintChars(NewSpan((Ref<const ubyte>)it.ref(), it.current_char_length()));
        }
    } // namespace Utf8
} // namespace Nrl
