#pragma once

#include "./Chars.hpp"
#include "./AlignedHeapAllocator.hpp"

namespace Nrl {
    template<typename T>
    class Debug;

    enum class StringState : u8 {
        None = 0, Long, Short
    };

    constexpr usize k_ShortStringMaxSize = 23;

    class String {
    public:
        using ValueType = utf8rune;
        using ConstIterator = Utf8::Iterator;
    public:
        [[nodiscard]] constexpr static String Empty(void) { return String(); }
        [[nodiscard]] constexpr static String Reserve(usize capacity) {
            String str;
            str.reserve(capacity);
            return str;
        }

        template<c_Char T>
        [[nodiscard]] static String New(Span<T> chars) {
            String str;
            str.set(chars);
            return str;
        }

        template<c_Char T>
        [[nodiscard]] static String Of(const T* chars) {
            String str;
            str.set(Chars::Of(chars));
            return str;
        }

        ~String(void) { dispose(); }

        String(const String& other) {
            if (other.is_empty())
                _empty();
            else
                set(other.chars());
        }
        String& operator=(const String& other) {
            if (this == &other)
                return *this;

            if (other.is_empty()) {
                dispose();
            } else {
                clear();
                set(other.chars());
            }

            return *this;
        }

        String(String&& other) {
            memcpy((void*)this, (void*)&other, sizeof(String));
            other._empty();
        }
        String& operator=(String&& other) {
            if (this == &other)
                return *this;

            clear();
            if (m_State == StringState::Long)
                m_Allocator.dealloc(m_Long.data, m_Long.max_size);

            memcpy((void*)this, (void*)&other, sizeof(String));
            other._empty();

            return *this;
        }

        template<c_Char T>
        [[nodiscard]] String operator+(Span<T> chars) const { return cat(chars); }

        template<c_Char T>
        [[nodiscard]] String operator+(T c) const { return cat(c); }

        [[nodiscard]] String operator+(const String& other) const { return cat(other.chars()); }

        template<c_Char T>
        String& operator+=(Span<T> chars) { append(chars); return *this; }

        template<c_Char T>
        String& operator+=(T c) { push(c); return *this; }

        String& operator+=(const String& other) { append(other.chars()); return *this; }

        template<c_Char T>
        [[nodiscard]] String cat(Span<T> chars) const {
            String str;
            str.reserve(max_size() + chars.length());
            str.set_data(this->chars());
            str.set_data(chars, size());
            return str;
        }

        template<c_Char T>
        [[nodiscard]] String cat(T c) const {
            String str;
            str.reserve(max_size() + 4);
            str.set_data(this->chars());
            *(str.ref() + str.size()) = c;
            str.set_size(size() + 1);
        }

        template<c_Char T>
        void append(Span<T> chars) {
            grow(chars.length());
            set_data(chars, size());
        }

        template<c_Char T>
        void push(T c) {
            reserve(max_size() + 4);
            *(ref() + size()) = c;
            set_size(size() + 1);
        }

        template<c_Char T>
        void set_data(Span<T> chars, usize offset = 0) {
            memcpy(ref().ptr() + offset, chars.ref().ptr(), chars.length());
            set_size(chars.length() + offset);
        }

        template<c_Char T>
        void set(Span<T> chars, usize offset = 0) {
            reserve(chars.length());
            set_data(chars, offset);
        }

        void clear(void) {
            set_size(0);
        }

        void set_size(usize new_size) {
            switch (m_State) {
                case StringState::Long:
                    m_Long.size = new_size;
                    break;
                case StringState::Short:
                    m_Short.size = new_size;
                    break;
                default:
            }
        }

        void reserve(usize new_max_size) {
            NRL_ASSERT(new_max_size >= size(), "Could not reserve memory for String: new max_size is less than current size!");

            if (new_max_size <= k_ShortStringMaxSize) {
                if (!is_long())
                    m_State = StringState::Short;
                return;
            }

            if (new_max_size == max_size())
                return;

            auto new_data = m_Allocator.alloc(new_max_size);
            switch (m_State) {
                case StringState::Long:
                    memcpy(new_data.ptr(), m_Long.data.ptr(), m_Long.size);
                    m_Allocator.dealloc(m_Long.data, m_Long.max_size);
                    break;
                case StringState::Short:
                    memcpy(new_data.ptr(), m_Short.data, m_Short.size);
                    m_Long.size = m_Short.size;
                    break;
                default:
                    m_Long.size = 0;
            }
            m_State = StringState::Long;
            m_Long.data = new_data;
            m_Long.max_size = new_max_size;
        }
        void grow(usize n) {
            reserve(max_size() + n);
        }
        void grow_by(float r) {
            reserve((usize)(max_size() * r + 0.5f));
        }

        void dispose(void) {
            clear();
            if (m_State == StringState::Long)
                m_Allocator.dealloc(m_Long.data, m_Long.max_size);
            _empty();
        }

        [[nodiscard]] constexpr utf8rune operator[](usize i) const { return *at(i); }

        [[nodiscard]] constexpr ConstIterator at(usize i) const { return ConstIterator(ref()) + i; }
        [[nodiscard]] constexpr ConstIterator begin(void) const { return ref(); }
        [[nodiscard]] constexpr ConstIterator end(void) const { return ref() + size(); }

        [[nodiscard]] usize constexpr get_length(void) const {
            if (is_empty())
                return 0;
            else
                return Utf8::GetStringLength(chars());
        }

        [[nodiscard]] usize constexpr size(void) const {
            switch (m_State) {
                case StringState::Long:  return m_Long.size;
                case StringState::Short: return m_Short.size;
                default:                 return 0;
            }
        }

        [[nodiscard]] usize constexpr max_size(void) const {
            switch (m_State) {
                case StringState::Long:  return m_Long.max_size;
                case StringState::Short: return k_ShortStringMaxSize;
                default:                 return 0;
            }
        }

        [[nodiscard]] constexpr bool is_ascii(void) const { return Utf8::IsCharsAscii(chars()); }

        [[nodiscard]] constexpr Span<utf8char> chars(void) {
            NRL_ASSERT(!is_empty(), "Could not get span to String: empty!");

            if (is_short())
                return NewSpan(RefFromPtr(m_Short.data), m_Short.size);
            else
                return NewSpan(m_Long.data, m_Long.size);
        }

        [[nodiscard]] constexpr Span<const utf8char> chars(void) const {
            NRL_ASSERT(!is_empty(), "Could not get span to String: empty!");

            if (is_short())
                return NewSpan(RefFromPtr(m_Short.data), m_Short.size);
            else
                return NewSpan(m_Long.data, m_Long.size);
        }

        [[nodiscard]] constexpr Ref<utf8char> ref(void) {
            NRL_ASSERT(!is_empty(), "Could not get ref to String: empty!");

            if (is_short())
                return RefFromPtr(m_Short.data);
            else
                return m_Long.data;
        }
        [[nodiscard]] constexpr Ref<const utf8char> ref(void) const {
            NRL_ASSERT(!is_empty(), "Could not get ref to String: empty!");

            if (is_short())
                return RefFromPtr(m_Short.data);
            else
                return m_Long.data;
        }

        [[nodiscard]] constexpr bool is_empty(void) const { return size() == 0; }
        [[nodiscard]] constexpr bool is_long(void)  const { return m_State == StringState::Long; }
        [[nodiscard]] constexpr bool is_short(void) const { return m_State == StringState::Short; }

        [[nodiscard]] constexpr StringState state(void) const { return m_State; }
    private:
        friend class Debug<String>;

        void _empty(void) { memset((void*)this, 0, sizeof(String)); }

        String(void) { _empty(); }
    private:
        union {
            struct {
                Ref<utf8char> data;
                usize size, max_size;
            } m_Long;
            struct {
                usize size;
                utf8char data[k_ShortStringMaxSize];
                ubyte padding_;
            } m_Short;
            struct {
                ubyte padding_[k_ShortStringMaxSize + sizeof(usize)];
                StringState m_State;
            };
        };
        NRL_NO_UNIQUE_ADDRESS Allocator<utf8char> m_Allocator = Allocator<utf8char>::New();
    };

    [[nodiscard]] constexpr String operator""s(const char* chars, usize size) {
        return String::New(NewSpan(RefFromPtr(chars), size));
    }

    [[nodiscard]] constexpr String operator""s(const utf8char* chars, usize size) {
        return String::New(NewSpan(RefFromPtr(chars), size));
    }
} // namespace Nrl
