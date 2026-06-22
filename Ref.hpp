#pragma once

#include "./Utils.hpp"
#include "./Debug.hpp"
#include "nrl/Traits.hpp"

namespace Nrl {
    // non owning, non zero reference (unless inside Option)
    template<typename T>
    class Ref {
    public:
        [[nodiscard]] static Ref New(T& referenced) {
            return &referenced;
        }
        [[nodiscard]] static Ref FromPtr(T* ptr) {
            NRL_ASSERT(ptr, "Failed to create Ref: ptr is null!");
            return ptr;
        }

        ~Ref(void) { m_Ptr = nullptr; }

        Ref(const Ref& other) : m_Ptr(other.m_Ptr) {}
        Ref& operator=(const Ref& other) {
            if (this == &other)
                return *this;

            m_Ptr = other.m_Ptr;

            return *this;
        }

        Ref(Ref&& other) noexcept : m_Ptr(Exchange(other.m_Ptr, nullptr)) {}
        Ref& operator=(Ref&& other) noexcept {
            if (this == &other)
                return *this;

            m_Ptr = Exchange(other.m_Ptr, nullptr);

            return *this;
        }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Ptr; }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ptr; }
		[[nodiscard]] constexpr T* operator->(void) const { return m_Ptr; }

		[[nodiscard]] constexpr bool operator==(Ref other) const { return m_Ptr == other.m_Ptr; }
		[[nodiscard]] constexpr bool operator!=(Ref other) const { return m_Ptr != other.m_Ptr; }
		[[nodiscard]] constexpr bool operator> (Ref other) const { return m_Ptr >  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator< (Ref other) const { return m_Ptr <  other.m_Ptr; }
		[[nodiscard]] constexpr bool operator>=(Ref other) const { return m_Ptr >= other.m_Ptr; }
		[[nodiscard]] constexpr bool operator<=(Ref other) const { return m_Ptr <= other.m_Ptr; }

		constexpr Ref& operator++(void) { m_Ptr++; return *this; }
		constexpr Ref  operator++(int) { m_Ptr++; return m_Ptr - 1; }

		constexpr Ref& operator--(void) { m_Ptr--; return *this; }
		constexpr Ref  operator--(int) { m_Ptr--; return m_Ptr + 1; }

		constexpr Ref& operator+=(usize x) { m_Ptr += x; return *this; }
		constexpr Ref& operator-=(usize x) { m_Ptr -= x; return *this; }

		[[nodiscard]] constexpr Ref operator+(usize x) const { return m_Ptr + x; }
		[[nodiscard]] constexpr Ref operator-(usize x) const { return m_Ptr - x; }

		[[nodiscard]] constexpr iptr operator-(Ref other) const { return m_Ptr - other.m_Ptr; }

		template<typename U>
		    requires IsConvertible_v<T*, U*>
		[[nodiscard]] operator Ref<U>(void) const { return Ref<U>::FromPtr((U*)m_Ptr); }

		template<typename U>
		[[nodiscard]] explicit operator Ref<U>(void) const { return Ref<U>::FromPtr((U*)m_Ptr); }

        [[nodiscard]] constexpr static Ref _None(void) { return nullptr; }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
    private:
        Ref(T* ptr) : m_Ptr(ptr) {}
    private:
        T* m_Ptr;
    };

    template<typename T>
    [[nodiscard]] Ref<T> NewRef(T& referenced) { return Ref<T>::New(referenced); }

    template<typename T>
    [[nodiscard]] Ref<T> RefFromPtr(T* ptr) { return Ref<T>::FromPtr(ptr); }
} // namespace Nrl
