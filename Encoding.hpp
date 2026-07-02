#pragma once

#include "./Option.hpp"
#include "./String.hpp"
#include "./StringView.hpp"
#include "./Tray.hpp"

namespace Nrl {
    enum class TextEncoding : u8 {
        Binary = 1, // {:b}
        Octal,      // {:o}
        Decimal,    // {:d}
        Hex,        // {:x}
        Base64,     // {:s}
        Utf8        // {:t}
    };

    [[nodiscard]] constexpr String EncodeBinary(
        Span<const ubyte> bytes,
        Option<char> padding = Some(' ')
    ) {
        auto out = String::Reserve(bytes.length() * (8 + padding.is_some()));

        for (usize i = 0; ubyte byte : bytes) {
            for (usize j = 0; j < 8; j++) {
                if (byte & (1 << (7 - j)))
                    out.push('1');
                else
                    out.push('0');
            }

            if (padding.is_some()) {
                if (++i != bytes.length())
                    out.push(padding.unwrap());
            }
        }

        return out;
    }
    [[nodiscard]] constexpr String EncodeOctal(
        Span<const ubyte> bytes,
        Option<char> padding = Some(' ')
    ) {
        auto out = String::Reserve(bytes.length() * (3 + padding.is_some()));

        for (usize i = 0; ubyte byte : bytes) {
            out.push((char)('0' + ((byte >> 6) & 0x07)));
            out.push((char)('0' + ((byte >> 3) & 0x07)));
            out.push((char)('0' + (byte & 0x07)));

            if (padding.is_some()) {
                if (++i != bytes.length())
                    out.push(padding.unwrap());
            }
        }

        return out;
    }
    [[nodiscard]] constexpr String EncodeDecimal(
        Span<const ubyte> bytes,
        Option<char> padding = Some(' ')
    ) {
        auto out = String::Reserve(bytes.length() * (3 + padding.is_some()));

        for (usize i = 0; ubyte byte : bytes) {
            out.push(char('0' + (byte / 100)));
            out.push(char('0' + ((byte / 10) % 10)));
            out.push(char('0' + (byte % 10)));

            if (padding.is_some()) {
                if (++i != bytes.length())
                    out.push(padding.unwrap());
            }
        }

        return out;
    }
    [[nodiscard]] constexpr String EncodeHex(
        Span<const ubyte> bytes,
        Option<char> padding = Some(' ')
    ) {
        static const char x_Table[] = "0123456789abcdef";

        auto out = String::Reserve(bytes.length() * (2 + padding.is_some()));

        for (usize i = 0; ubyte byte : bytes) {
            out.push(x_Table[(byte >> 4) & 0x0F]);
            out.push(x_Table[byte & 0x0F]);

            if (padding.is_some()) {
                if (++i != bytes.length())
                    out.push(padding.unwrap());
            }
        }

        return out;
    }
    [[nodiscard]] constexpr String EncodeBase64(Span<const ubyte> bytes) {
        static const char x_Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        auto out = String::Reserve(4 * ((bytes.length() + 2) / 3));

        for (usize i = 0; i < bytes.length(); i += 3) {
            u32 octet_a = bytes[i];
            u32 octet_b = (i + 1 < bytes.length()) ? bytes[i + 1] : 0;
            u32 octet_c = (i + 2 < bytes.length()) ? bytes[i + 2] : 0;

            u32 triple = (octet_a << 16) | (octet_b << 8) | octet_c;

            out.push(x_Table[(triple >> 18) & 0x3F]);
            out.push(x_Table[(triple >> 12) & 0x3F]);

            if (i + 1 < bytes.length())
                out.push(x_Table[(triple >> 6) & 0x3F]);
            else
                out.push('=');

            if (i + 2 < bytes.length())
                out.push(x_Table[triple & 0x3F]);
            else
                out.push('=');
        }

        return out;
    }
    [[nodiscard]] constexpr String EncodeUtf8(Span<const ubyte> bytes) {
        return String::New(bytes);
    }

    [[nodiscard]] constexpr String Encode(
        Span<const ubyte> bytes,
        TextEncoding encoding,
        Option<char> padding = Some(' ')
    ) {
        switch (encoding) {
            case TextEncoding::Binary:  return EncodeBinary(bytes, padding);
            case TextEncoding::Octal:   return EncodeOctal(bytes, padding);
            case TextEncoding::Decimal: return EncodeDecimal(bytes, padding);
            case TextEncoding::Hex:     return EncodeHex(bytes, padding);
            case TextEncoding::Base64:  return EncodeBase64(bytes);
            case TextEncoding::Utf8:    return EncodeUtf8(bytes);
        }
    }

    [[nodiscard]] constexpr Tray<ubyte> DecodeBinary(StringView str, bool padding) {
        auto chars = str.chars();

        usize chunk_size = 8 + padding;
        usize byte_count = (chars.length() + padding) / chunk_size;

        auto bytes = Tray<ubyte>::Reserve(byte_count);

        for (usize i = 0; i < byte_count; i++) {
            ubyte byte = 0;
            usize offset = i * chunk_size;

            for (usize j = 0; j < 8; j++) {
                char c = chars[offset + j];
                if (c == '1')
                    byte |= (1 << (7 - j));
            }
            bytes.emplace(byte);
        }

        return bytes;
    }

    [[nodiscard]] constexpr Tray<ubyte> DecodeOctal(StringView str, bool padding) {
        auto chars = str.chars();

        usize chunk_size = 3 + padding;
        usize byte_count = (chars.length() + padding) / chunk_size;

        auto bytes = Tray<ubyte>::Reserve(byte_count);

        for (usize i = 0; i < byte_count; i++) {
            usize offset = i * chunk_size;
            ubyte byte = 0;

            byte |= (ubyte)(chars[offset] - '0') << 6;
            byte |= (ubyte)(chars[offset + 1] - '0') << 3;
            byte |= (ubyte)(chars[offset + 2] - '0');

            bytes.emplace(byte);
        }

        return bytes;
    }
    [[nodiscard]] constexpr Tray<ubyte> DecodeDecimal(StringView str, bool padding) {
        auto chars = str.chars();

        usize chunk_size = 3 + padding;
        usize byte_count = (chars.length() + padding) / chunk_size;

        auto bytes = Tray<ubyte>::Reserve(byte_count);

        for (usize i = 0; i < byte_count; i++) {
            usize offset = i * chunk_size;

            ubyte x = 0;
            x += (ubyte)(chars[offset] - '0') * 100;
            x += (ubyte)(chars[offset + 1] - '0') * 10;
            x += (ubyte)(chars[offset + 2] - '0');

            bytes.emplace(x);
        }

        return bytes;
    }

    [[nodiscard]] constexpr ubyte HexCharToValue(char c) {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;

        return 0;
    }
    [[nodiscard]] constexpr Tray<ubyte> DecodeHex(StringView str, bool padding) {
        auto chars = str.chars();

        usize chunk_size = 2 + padding;
        usize byte_count = (chars.length() + padding) / chunk_size;

        auto bytes = Tray<ubyte>::Reserve(byte_count);

        for (usize i = 0; i < byte_count; i++) {
            usize offset = i * chunk_size;

            ubyte high = HexCharToValue(chars[offset]);
            ubyte low = HexCharToValue(chars[offset + 1]);

            bytes.emplace((high << 4) | low);
        }

        return bytes;
    }

    [[nodiscard]] constexpr u32 Base64CharToValue(char c) {
        if (c >= 'A' && c <= 'Z')
            return c - 'A';
        if (c >= 'a' && c <= 'z')
            return c - 'a' + 26;
        if (c >= '0' && c <= '9')
            return c - '0' + 52;
        if (c == '+')
            return 62;
        if (c == '/')
            return 63;

        return 0;
    }
    [[nodiscard]] constexpr Tray<ubyte> DecodeBase64(StringView str) {
        auto chars = str.chars();

        usize padding_count = 0;
        if (chars.length() > 0) {
            if (chars[chars.length() - 1] == '=')
                padding_count++;
        }
        if (chars.length() > 1) {
            if (chars[chars.length() - 2] == '=')
                padding_count++;
        }

        usize max_bytes = chars.length() / 4 * 3;
        usize byte_count = max_bytes - padding_count;

        auto bytes = Tray<ubyte>::Reserve(byte_count);

        for (usize i = 0; i < chars.length(); i += 4) {
            u32 a = Base64CharToValue(chars[i]);

            u32 b = 0;
            if (i + 1 < chars.length())
                b = Base64CharToValue(chars[i + 1]);

            u32 c = 0;
            if (i + 2 < chars.length())
                c = Base64CharToValue(chars[i + 2]);

            u32 d = 0;
            if (i + 3 < chars.length())
                d = Base64CharToValue(chars[i + 3]);

            u32 triple = (a << 18) | (b << 12) | (c << 6) | d;

            if (bytes.length() < byte_count)
                bytes.emplace((triple >> 16) & 0xFF);

            if (bytes.length() < byte_count)
                bytes.emplace((triple >> 8) & 0xFF);

            if (bytes.length() < byte_count)
                bytes.emplace(triple & 0xFF);
        }

        return bytes;
    }
    [[nodiscard]] constexpr Tray<ubyte> DecodeUtf8(StringView str) {
        auto bytes = Tray<ubyte>::Reserve(str.size());
        RefCopy(bytes.ref(), str.ref(), str.size());
        bytes.set_length(str.size());
        return bytes;
    }

    [[nodiscard]] constexpr Tray<ubyte> Decode(
        StringView str,
        TextEncoding encoding,
        bool padding
    ) {
        switch (encoding) {
            case TextEncoding::Binary:  return DecodeBinary(str, padding);
            case TextEncoding::Octal:   return DecodeOctal(str, padding);
            case TextEncoding::Decimal: return DecodeDecimal(str, padding);
            case TextEncoding::Hex:     return DecodeHex(str, padding);
            case TextEncoding::Base64:  return DecodeBase64(str);
            case TextEncoding::Utf8:    return DecodeUtf8(str);
        }
    }

    [[nodiscard]] constexpr char TextEncoding_GetFormat(TextEncoding encoding) {
        switch (encoding) {
            case TextEncoding::Binary:  return 'b';
            case TextEncoding::Octal:   return 'o';
            case TextEncoding::Decimal: return 'd';
            case TextEncoding::Hex:     return 'x';
            case TextEncoding::Base64:  return 's';
            case TextEncoding::Utf8:    return 't';
        }
    }
    [[nodiscard]] constexpr Option<TextEncoding> TextEncoding_FromFormat(char c) {
        switch (c) {
            case 'b': return Some(TextEncoding::Binary);
            case 'o': return Some(TextEncoding::Octal);
            case 'd': return Some(TextEncoding::Decimal);
            case 'x': return Some(TextEncoding::Hex);
            case 's': return Some(TextEncoding::Base64);
            case 't': return Some(TextEncoding::Utf8);
            default:  return None();
        }
    }

    [[nodiscard]] constexpr StringView TextEncoding_GetPrefix(TextEncoding encoding) {
        switch (encoding) {
            case TextEncoding::Binary:  return "0b"sv;
            case TextEncoding::Octal:   return "0o"sv;
            case TextEncoding::Decimal: return "0d"sv;
            case TextEncoding::Hex:     return "0x"sv;
            case TextEncoding::Base64:  return ""sv;
            case TextEncoding::Utf8:    return ""sv;
        }
    }
} // namespace Nrl
