#pragma once

#include "./Span.hpp"
#include "./Utf8.hpp"
#include "./Option.hpp"

namespace Nrl {
    namespace Chars {
        template<c_Char T>
        [[nodiscard]] constexpr Span<T> Of(T* chars) {
            Ref<T> ref = RefFromPtr(chars);
            return NewSpan(ref, Utf8::GetStringSize(ref));
        }

        template<typename T>
        [[nodiscard]] constexpr Option<typename T::Iterator> Find(T chars, typename T::ValueType c) {
            for (auto it = chars.begin(); it != chars.end(); it++) {
                if (*it == c)
                    return Some(it);
            }
            return None();
        }

        [[nodiscard]] constexpr Option<Utf8::Iterator> FindRune(Subrange<Utf8::Iterator> string, utf8rune rune) {
            for (auto it = string.begin(); it != string.end(); it++) {
                if (*it == rune)
                    return Some(it);
            }
            return None();
        }

        template<c_Char T>
        [[nodiscard]] constexpr bool IsTrimmable(T c) {
            return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
        }
    } // namespace Chars
} // namespace Nrl
