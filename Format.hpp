#pragma once

#include <inttypes.h>

#include "./String.hpp"
#include "./StringView.hpp"
#include "./Pool.hpp"
#include "./Ref.hpp"

namespace Nrl {
    template<typename T>
    class Display {
    public:
        [[nodiscard]] static String ToString(const T& x, StringView params) {
            static_assert(false, "Display<T> is not implemented!");
            return String::Of("[Unimplemented Display]");
        }
    };

    template<typename T>
    class Debug {
    public:
        [[nodiscard]] static String ToString(const T& x, StringView params) {
            return Display<T>::ToString(x, params);
        }
    };

    struct FormatArgument {
        Ref<const void> data;
        String (*to_string)(Ref<const void> data, bool is_debug, StringView params);

        template<typename T>
        [[nodiscard]] static String ToString(Ref<const void> d, bool is_debug, StringView params) {
            const T& x = *(Ref<const T>)d;

            if (is_debug)
                return Debug<T>::ToString(x, params);
            else
                return Display<T>::ToString(x, params);
        }

        template<typename T>
        FormatArgument(const T& data) : data(NewRef(data)), to_string(&ToString<T>)
        {}
    };

    [[nodiscard]] String VFormat(const char* fmt, Span<FormatArgument> args) {
        auto out = String::Empty();

        while (*fmt != '\0') {
            if (*fmt == '{') {
                fmt++;

                usize index = 0;
                while (*fmt >= '0' && *fmt <= '9') {
                    index = (index * 10) + (*fmt - '0');
                    fmt++;
                }

                bool is_debug = false;
                auto parameters = Pool<char, 32>::Empty();

                if (*fmt == ':') {
                    fmt++;
                    if (*fmt == '?') {
                        is_debug = true;
                        fmt++;
                    }

                    while (*fmt != '}' && *fmt != '\0' && parameters.length() < 31) {
                        parameters.emplace(*fmt);
                        fmt++;
                    }
                }

                if (*fmt == '}') {
                    if (index < args.length()) {
                        const FormatArgument& arg = args[index];

                        String formatted = arg.to_string(
                            arg.data,
                            is_debug,
                            StringView::New(SpanOf(parameters))
                        );

                        out += formatted;
                    } else {
                        NRL_ASSERT(false, "Failed to format string: Index out of bounds!");
                        out += String::Of("#OOB#");
                    }
                    fmt++;
                }
            } else {
                out.push(*fmt);
                fmt++;
            }
        }
        return out;
    }

    template<typename... Args>
    [[nodiscard]] String Format(const char* fmt, const Args&... args) {
        FormatArgument format_args[] = { FormatArgument(args)... };
        return VFormat(fmt, NewSpan(RefFromPtr(format_args), sizeof...(Args)));
    }

    template<typename... Args>
    void Printf(const char* fmt, const Args&... args) {
        auto str = Format(fmt, args...);
        Utf8::Print(ConstSubrangeOf(str));
    }

    template<typename... Args>
    void Printfln(const char* fmt, const Args&... args) {
        Printf(fmt, args...);
        puts("\n");
    }

    template<typename T>
    [[nodiscard]] String Format1(const char* fmt, const T& arg, StringView params) {
        auto out = String::Empty();

        bool is_debug = params.starts_with('?');

        auto formatted = String::Empty();

        if (is_debug) {
            params = params.sub(1, 0);
            formatted = Debug<T>::ToString(arg, params);
        } else {
            formatted = Display<T>::ToString(arg, params);
        }

        while (*fmt != '\0') {
            if (*fmt == '{') {
                fmt++;
                out += formatted;
                fmt++;
            } else {
                out.push(*fmt);
                fmt++;
            }
        }

        return out;
    }
} // namespace Nrl

#include "./Formatters.hpp"
