#pragma once

#include "./Format.hpp"
#include "./Encoding.hpp"

namespace Nrl {
    template<typename T>
    class Display<const T> {
    public:
        [[nodiscard]] static String ToString(const T& x, StringView params) {
            return Display<T>::ToString(x, params);
        }
    };
    template<typename T>
    class Debug<const T> {
    public:
        [[nodiscard]] static String ToString(const T& x, StringView params) {
            return Debug<T>::ToString(x, params);
        }
    };

    struct ParsedEncoding {
        TextEncoding encoding;
        Option<char> padding;
        bool do_prefix;
    };
    [[nodiscard]] constexpr Option<ParsedEncoding> ParseEncoding(StringView params) {
        bool do_prefix = params.starts_with('0');
        if (do_prefix)
            params = params.sub(1, 0);

        if (params.is_empty())
            return None();

        auto encoding_opt = TextEncoding_FromFormat(params.chars()[0]);
        if (encoding_opt.is_none())
            return None();
        else
            params = params.sub(1, 0);

        auto encoding = encoding_opt.unwrap();

        if (params.is_empty())
            return Some(ParsedEncoding {
                .encoding = encoding,
                .padding = None(),
                .do_prefix = do_prefix
            });
        else
            return Some(ParsedEncoding {
                .encoding = encoding,
                .padding = Some((char)params[0]),
                .do_prefix = do_prefix
            });
    }
    [[nodiscard]] constexpr Option<u32> ParseFloatPrecision(StringView params) {
        auto chars = params.chars();

        u32 precision = u32max;

        if (chars.length() >= 2 && chars[0] == '.') {
            precision = 0;
            usize i = 1;
            while (i < chars.length() && chars[i] >= '0' && chars[i] <= '9') {
                precision = precision * 10 + (chars[i] - '0');
                i++;
            }
        }

        if (precision != u32max)
            return Some(precision);
        else
            return None();
    }

    [[nodiscard]] String FormatFloat(f64 x, StringView params) {
        u32 precision = ParseFloatPrecision(params).unwrap_or(6);

        char chars[64];
        usize length = (usize)snprintf(chars, sizeof(chars), "%.*f", precision, x);
        return String::New(NewSpan(RefFromPtr(chars), length));
    }

    template <typename T>
    [[nodiscard]] inline String FormatEncodedInteger(T x, ParsedEncoding encoding) {
        ubyte bytes[sizeof(T)];
        usize offset = sizeof(T);

        for (usize i = 0; i < sizeof(T); i++) {
            bytes[i] = (ubyte)((x >> ((sizeof(T) - 1 - i) * 8)) & 0xFF);
            if (offset == sizeof(T) && bytes[i] != 0) {
                offset = i;
            }
        }

        if (offset == sizeof(T))
            offset = sizeof(T) - 1;

        auto active_bytes = NewSpan(RefFromPtr(bytes + offset), sizeof(T) - offset);

        auto encoded = Encode(active_bytes, encoding.encoding, encoding.padding);

        if (encoding.do_prefix) {
            if (encoding.padding.is_some()) {
                return Format("{0}({1})", TextEncoding_GetPrefix(encoding.encoding), encoded);
            } else {
                return Format("{0}{1}", TextEncoding_GetPrefix(encoding.encoding), encoded);
            }
        } else {
            return encoded;
        }
    }

    template<>
    class Display<bool> {
    public:
        using ValueType = bool;

        [[nodiscard]] static String ToString(bool x, StringView params) {
            if (x)
                return String::Of("true");
            else
                return String::Of("false");
        }
    };

    template<>
    class Debug<bool> {
    public:
        using ValueType = bool;

        [[nodiscard]] static String ToString(bool x, StringView params) {
            if (x)
                return String::Of("bool(1)");
            else
                return String::Of("bool(0)");
        }
    };

    template<typename T>
    struct IntegerMaxChars;
    template<>
    struct IntegerMaxChars<i8>  { constexpr static usize value = 4; };
    template<>
    struct IntegerMaxChars<u8>  { constexpr static usize value = 3; };
    template<>
    struct IntegerMaxChars<i16> { constexpr static usize value = 6; };
    template<>
    struct IntegerMaxChars<u16> { constexpr static usize value = 5; };
    template<>
    struct IntegerMaxChars<i32> { constexpr static usize value = 11; };
    template<>
    struct IntegerMaxChars<u32> { constexpr static usize value = 10; };
    template<>
    struct IntegerMaxChars<i64> { constexpr static usize value = 20; };
    template<>
    struct IntegerMaxChars<u64> { constexpr static usize value = 20; };

    template<typename T>
    constexpr static usize IntegerMaxChars_v = IntegerMaxChars<T>::value;

    #define NRL_IMPL_INTEGER_DISPLAY(TYPE, FMT) \
    template<>\
    class Display<TYPE> { \
    public: \
        using ValueType = TYPE; \
        \
        [[nodiscard]] static String ToString(TYPE x, StringView params) { \
            auto parsed_encoding = ParseEncoding(params); \
            if (parsed_encoding.is_some()) \
                return FormatEncodedInteger<TYPE>(x, parsed_encoding.unwrap()); \
            \
            char chars[IntegerMaxChars_v<TYPE> + 1]; \
            usize length = (usize)snprintf(chars, sizeof(chars), "%" FMT, x); \
            return String::New(NewSpan(RefFromPtr(chars), length)); \
        }\
    }

    #define NRL_IMPL_INTEGER_DEBUG(TYPE, NAME, FMT) \
    template<>\
    class Debug<TYPE> { \
    public: \
        using ValueType = TYPE; \
        \
        [[nodiscard]] static String ToString(TYPE x, StringView params) { \
            auto parsed_encoding = ParseEncoding(params); \
            if (parsed_encoding.is_some()) \
                return Format1(NAME "({})", FormatEncodedInteger<TYPE>(x, parsed_encoding.unwrap()), ""sv); \
            else \
                return Format1(NAME "({})", x, params);\
        } \
    }

    #define NRL_IMPL_INTEGER_FORMATTERS(TYPE, NAME, FMT) NRL_IMPL_INTEGER_DISPLAY(TYPE, FMT); NRL_IMPL_INTEGER_DEBUG(TYPE, NAME, FMT)

    NRL_IMPL_INTEGER_FORMATTERS(i8,  "i8",  PRId8);
    NRL_IMPL_INTEGER_FORMATTERS(i16, "i16", PRId16);
    NRL_IMPL_INTEGER_FORMATTERS(i32, "i32", PRId32);
    NRL_IMPL_INTEGER_FORMATTERS(i64, "i64", PRId64);
    NRL_IMPL_INTEGER_FORMATTERS(u8,  "u8",  PRIu8);
    NRL_IMPL_INTEGER_FORMATTERS(u16, "u16", PRIu16);
    NRL_IMPL_INTEGER_FORMATTERS(u32, "u32", PRIu32);
    NRL_IMPL_INTEGER_FORMATTERS(u64, "u64", PRIu64);

    #define NRL_IMPL_FLOAT_DISPLAY(TYPE) \
    template<> \
    class Display<TYPE> { \
    public: \
        using ValueType = TYPE; \
        \
        [[nodiscard]] static String ToString(TYPE x, StringView params) { \
            return FormatFloat((f64)x, params); \
        } \
    }

    #define NRL_IMPL_FLOAT_DEBUG(TYPE, NAME) \
    template<> \
    class Debug<TYPE> { \
    public: \
        using ValueType = TYPE; \
        \
        [[nodiscard]] static String ToString(TYPE x, StringView params) { \
            return Format1(NAME "({})", x, params); \
        } \
    }

    #define NRL_IMPL_FLOAT_FORMATTERS(TYPE, NAME) NRL_IMPL_FLOAT_DISPLAY(TYPE); NRL_IMPL_FLOAT_DEBUG(TYPE, NAME)

    NRL_IMPL_FLOAT_FORMATTERS(f32, "f32");
    NRL_IMPL_FLOAT_FORMATTERS(f64, "f64");

    template<typename T>
    class Display<T*> {
    public:
        using ValueType = T*;

        [[nodiscard]] static String ToString(T* x, StringView params) {
            if (params.is_empty())
                params = "0x"sv;

            return Display<usize>::ToString((usize)x, params);
        }
    };

    template<typename T>
    class Display<Ref<T>> {
    public:
        using ValueType = Ref<T>;

        [[nodiscard]] static String ToString(Ref<T> x, StringView params) {
            return Display<T*>::ToString(x.ptr(), params);
        }
    };

    template<typename T>
    class Debug<Ref<T>> {
    public:
        using ValueType = Ref<T>;

        [[nodiscard]] static String ToString(Ref<T> x, StringView params) {
            return Format1("Ref({})", x.ptr(), params);
        }
    };

    template<c_InputIterator It>
    class Display<Subrange<It>> {
    public:
        using ValueType = Subrange<It>;

        [[nodiscard]] static String ToString(Subrange<It> x, StringView params) {
            auto out = String::Of("[");

            bool do_pretty = params.starts_with('^');
            if (do_pretty)
                params = params.sub(1, 0);

            bool do_inner_debug = params.starts_with("*?"sv);
            if (do_inner_debug)
                params = params.sub(2, 0);

            for (auto it = x.begin(); it != x.end(); it++) {
                if (do_pretty)
                    out += "\n\t"sv;

                if (do_inner_debug)
                    out += Debug<typename Subrange<It>::ValueType>::ToString(*it, params);
                else
                    out += Display<typename Subrange<It>::ValueType>::ToString(*it, params);

                if (it != x.end() - 1)
                    out += ", "sv;
                else
                if (do_pretty)
                    out.push('\n');
            }

            out.push(']');

            return out;
        }
    };

    template<c_InputIterator It>
    class Debug<Subrange<It>> {
    public:
        using ValueType = Subrange<It>;

        [[nodiscard]] static String ToString(Subrange<It> x, StringView params) {
            return Format("Subrange({0}, {1})", x.begin().ref(), x.end().ref());
        }
    };

    template<typename T>
    class Display<Span<T>> {
    public:
        using ValueType = Span<T>;

        [[nodiscard]] static String ToString(Span<T> x, StringView params) {
            using It = typename Span<T>::Iterator;
            return Display<Subrange<It>>::ToString(SubrangeOf(x), params);
        }
    };

    template<typename T>
    class Debug<Span<T>> {
    public:
        using ValueType = Span<T>;

        [[nodiscard]] static String ToString(Span<T> x, StringView params) {
            return Format("Span({0}, {1})", x.ref(), x.length());
        }
    };

    template<typename T>
        requires SameAs_v<const T, const ubyte>
    class Display<Span<T>> {
    public:
        using ValueType = Span<T>;

        [[nodiscard]] static String ToString(Span<T> x, StringView params) {
            auto parsed_encoding_opt = ParseEncoding(params);
            if (parsed_encoding_opt.is_none()) {
                using It = typename Span<T>::Iterator;
                return Display<Subrange<It>>::ToString(SubrangeOf(x), params);
            }

            auto parsed_encoding = parsed_encoding_opt.unwrap();
            return Encode(x, parsed_encoding.encoding, parsed_encoding.padding);
        }
    };

    template<>
    class Display<StringView> {
    public:
        using ValueType = StringView;

        [[nodiscard]] static String ToString(StringView x, StringView params) {
            return String::From(x);
        }
    };

    template<>
    class Debug<StringView> {
    public:
        using ValueType = StringView;

        [[nodiscard]] static String ToString(StringView x, StringView params) {
            return Format("StringView({0}, {1})", x.ref(), x.size());
        }
    };

    template<>
    class Display<String> {
    public:
        using ValueType = String;

        [[nodiscard]] static String ToString(const String& x, StringView params) {
            return x;
        }
    };

    template<>
    class Debug<String> {
    public:
        using ValueType = String;

        [[nodiscard]] static String ToString(const String& x, StringView params) {
            switch (x.state()) {
                case StringState::Long:  return Format("String(Long, data: {0}, size: {1}, max_size: {2})", x.m_Long.data, x.m_Long.size, x.m_Long.max_size);
                case StringState::Short: return Format("String(Short, size: {0})", x.m_Short.size);
                default:                 return Format("String(Empty)");
            }
        }
    };
} // namespace Nrl
