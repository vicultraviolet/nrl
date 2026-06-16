#pragma once

#include "./Utils.hpp"

namespace Nrl {
    // non owning, non zero reference (unless inside Option)
    template<typename T>
    class Ref {
    public:
        [[nodiscard]] static Ref New(T& referenced) {
            return &referenced;
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

		[[nodiscard]] constexpr auto operator<=>(const Ref& other) const { return m_Ptr <=> other.m_Ptr; }
		[[nodiscard]] constexpr auto operator==(const Ref& other) const { return m_Ptr == other.m_Ptr; }

		template<typename U>
		[[nodiscard]] explicit operator Ref<U>(void) { return Ref<U>::New(*(U*)m_Ptr); }

        [[nodiscard]] constexpr static Ref _None(void) { return nullptr; }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ptr; }
    private:
        Ref(T* ptr) : m_Ptr(ptr) {}
    private:
        T* m_Ptr;
    };

} // namespace Nrl
