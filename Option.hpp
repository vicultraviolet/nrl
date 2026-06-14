#pragma once

#include "./Debug.hpp"

#include "./Primitives.hpp"
#include "./Utils.hpp"

#include "./Pointers/Ref.hpp"

namespace Nrl {
    template<typename T>
    class NonIntrusiveOption;

    template<typename T>
    class IntrusiveOption;

    template<typename T>
    struct OptionAlias { using Type = IntrusiveOption<T>; };

    template<c_Optional T>
    struct OptionAlias<T> { using Type = NonIntrusiveOption<T>; };

    template<typename T>
    using Option = typename OptionAlias<T>::Type;

    class None_t {};

    template<typename T>
    class NonIntrusiveOption {
    public:
        template<typename F, typename... Args>
        [[nodiscard]] static NonIntrusiveOption SomeWith(F&& factory, Args&&... args) {
            return NonIntrusiveOption(factory, Forward<Args>(args)...);
        }

        template<typename... Args>
        [[nodiscard]] static NonIntrusiveOption Some(Args&&... args) {
            return SomeWith([](Args&&... args) -> T {
                return T(Forward<Args>(args)...);
            }, Forward<Args>(args)...);
        }

        [[nodiscard]] static NonIntrusiveOption None(void) {
            return NonIntrusiveOption(T::_None);
        }

        ~NonIntrusiveOption(void) = default;

        NonIntrusiveOption(const NonIntrusiveOption& other) = default;
        NonIntrusiveOption& operator=(const NonIntrusiveOption& other) = default;

        NonIntrusiveOption(NonIntrusiveOption&& other) noexcept = default;
        NonIntrusiveOption& operator=(NonIntrusiveOption&& other) noexcept = default;

        NonIntrusiveOption(const None_t& none) : NonIntrusiveOption(T::_None) {}
        NonIntrusiveOption& operator=(const None_t& none) { destroy(); }

        NonIntrusiveOption(None_t&& none) : NonIntrusiveOption(T::_None) {}
        NonIntrusiveOption& operator=(None_t&& none) { destroy(); }

        T unwrap(void) {
            NRL_ASSERT(is_some(), "Could not unwrap Option: No value!");
            return Move(m_Data);
        }

        void destroy(void) { m_Data.~T(); }

        [[nodiscard]] Option<Ref<T>> ref(void) {
            if (is_none()) {
                return Option<Ref<T>>::None();
            }
            return Option<Ref<T>>::SomeWith(Ref<T>::New, m_Data);
        }

        [[nodiscard]] Option<Ref<const T>> ref(void) const {
            if (is_none()) {
                return Option<Ref<const T>>::None();
            }
            return Option<Ref<const T>>::SomeWith(Ref<const T>::New, m_Data);
        }

        [[nodiscard]] constexpr bool is_some(void) const { return m_Data._is_some(); }
        [[nodiscard]] constexpr bool is_none(void) const { return !m_Data._is_some(); }
    private:
        template<typename F, typename... Args>
        NonIntrusiveOption(F&& factory, Args&&... args) : m_Data(factory(Forward<Args>(args)...)) {}
    private:
        T m_Data;
    };

    template<typename T>
    class IntrusiveOption {
    public:
        template<typename F, typename... Args>
        [[nodiscard]] static IntrusiveOption SomeWith(F&& factory, Args&&... args) {
            IntrusiveOption t;

            new (t._ptr()) T(factory(Forward<Args>(args)...));
            t.m_IsSome = true;

            return t;
        }

        template<typename... Args>
        [[nodiscard]] static IntrusiveOption Some(Args&&... args) {
            IntrusiveOption t;

            new (t._ptr()) T(Forward<Args>(args)...);
            t.m_IsSome = true;

            return t;
        }

        [[nodiscard]] constexpr static IntrusiveOption None(void) {
            return IntrusiveOption();
        }

        ~IntrusiveOption(void) {
            if (m_IsSome)
                _ptr()->~T();
        }

        IntrusiveOption(const IntrusiveOption& other)
        : m_IsSome(other.m_IsSome) {
            if (m_IsSome)
                new (_ptr()) T(*other._ptr());
        }

        IntrusiveOption& operator=(const IntrusiveOption& other) {
            if (this == &other)
                return *this;

            if (m_IsSome) {
                if (other.m_IsSome)
                    *_ptr() = *other._ptr();
                else
                    destroy();
            } else {
                if (other.m_IsSome) {
                    new (_ptr()) T(*other._ptr());
                    m_IsSome = true;
                }
            }

            return *this;
        }

        IntrusiveOption(IntrusiveOption&& other) noexcept
        : m_IsSome(other.m_IsSome) {
            if (m_IsSome) {
                new (_ptr()) T(Move(*other._ptr()));
                other.m_IsSome = false;
            }
        }

        IntrusiveOption& operator=(IntrusiveOption&& other) noexcept {
            if (this == &other)
                return *this;

            if (m_IsSome) {
                if (other.m_IsSome) {
                    *_ptr() = Move(*other._ptr());
                    other.m_IsSome = false;
                } else {
                    destroy();
                }
            } else {
                if (other.m_IsSome) {
                    new (_ptr()) T(Move(*other._ptr()));
                    other.m_IsSome = false;
                    m_IsSome = true;
                }
            }

            return *this;
        }

        IntrusiveOption(const None_t& none) : IntrusiveOption() {}
        IntrusiveOption& operator=(const None_t& none) { destroy(); }

        T unwrap(void) {
            NRL_ASSERT(m_IsSome, "Could not unwrap Option: No value!");
            m_IsSome = false;
            return Move(*_ptr());
        }

        void destroy(void) {
            if (m_IsSome) {
                _ptr()->~T();
                m_IsSome = false;
            }
        }

        [[nodiscard]] Option<Ref<T>> ref(void) {
            if (!m_IsSome) {
                return Option<Ref<T>>::None();
            }
            return Option<Ref<T>>::SomeWith(Ref<T>::New, *_ptr());
        }

        [[nodiscard]] Option<Ref<const T>> ref(void) const {
            if (!m_IsSome) {
                return Option<Ref<const T>>::None();
            }
            return Option<Ref<const T>>::SomeWith(Ref<const T>::New, *_ptr());
        }

        [[nodiscard]] constexpr bool is_some(void) const { return m_IsSome; }
        [[nodiscard]] constexpr bool is_none(void) const { return !m_IsSome; }
    private:
        IntrusiveOption(void) : m_Data{}, m_IsSome(false) {}

        [[nodiscard]] constexpr T* _ptr(void) { return (T*)m_Data; }
        [[nodiscard]] constexpr const T* _ptr(void) const { return (const T*)m_Data; }
    private:
        alignas(T) byte_t m_Data[sizeof(T)];
        bool m_IsSome;
    };

    template<typename T, typename... Args>
    constexpr Option<T> Some(Args&&... args) { return Option<T>::Some(Forward<Args>(args)...); }

    template<typename T, typename F, typename... Args>
    constexpr Option<T> SomeWith(F&& factory, Args&&... args) { return Option<T>::SomeWith(Forward<F>(factory), Forward<Args>(args)...); }

    constexpr None_t None(void) { return None_t{}; }

    template<typename T>
    constexpr Option<T> None(void) { return Option<T>::None(); }
} // namespace Nrl
