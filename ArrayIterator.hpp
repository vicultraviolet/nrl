#pragma once

#include "./Ref.hpp"

namespace Nrl {
    template<typename Array>
    class ArrayIterator {
	public:
	    using T = Array::ValueType;
	    using ValueType = Array::ValueType;
		using ArrayType = Array;
		using Reversed = Array::ReverseIterator;

        ArrayIterator(Ref<T> ref) : m_Ref(ref) {}

		constexpr auto& operator++(void) { m_Ref++; return *this; }
		constexpr auto  operator++(int) { return ArrayIterator(m_Ref++); }

		constexpr auto& operator--(void) { m_Ref--; return *this; }
		constexpr auto  operator--(int) { return ArrayIterator(m_Ref--); }

		constexpr auto& operator+=(size_t x) { m_Ref += x; return *this; }
		constexpr auto& operator-=(size_t x) { m_Ref -= x; return *this; }

		[[nodiscard]] constexpr ArrayIterator operator+(size_t x) const { return m_Ref + x; }
		[[nodiscard]] constexpr ArrayIterator operator-(size_t x) const { return m_Ref - x; }

		[[nodiscard]] constexpr T* operator->(void) const { return m_Ref.ptr(); }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ref; }

		[[nodiscard]] constexpr Ref<T> ref(void) { return m_Ref; }
		[[nodiscard]] constexpr T* ptr(void) { return m_Ref.ptr(); }

		[[nodiscard]] constexpr Reversed reverse(void) { return m_Ref; }

		[[nodiscard]] constexpr bool operator==(const ArrayIterator& other)  const { return m_Ref == other.m_Ref; }
		[[nodiscard]] constexpr auto operator<=>(const ArrayIterator& other) const { return m_Ref <=> other.m_Ref; }

		[[nodiscard]] static ArrayIterator _None(void) { return Ref<T>::_None(); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
	private:
	    Ref<T> m_Ref;
    };

    template<typename Array>
    class ArrayConstIterator {
	public:
	    using T = const Array::ValueType;
	    using ValueType = const Array::ValueType;
		using ArrayType = Array;
		using Reversed = Array::ConstReverseIterator;

        ArrayConstIterator(Ref<T> ref) : m_Ref(ref) {}

		constexpr auto& operator++(void) { m_Ref++; return *this; }
		constexpr auto  operator++(int) { return ArrayConstIterator(m_Ref++); }

		constexpr auto& operator--(void) { m_Ref--; return *this; }
		constexpr auto  operator--(int) { return ArrayConstIterator(m_Ref--); }

		constexpr auto& operator+=(size_t x) { m_Ref += x; return *this; }
		constexpr auto& operator-=(size_t x) { m_Ref -= x; return *this; }

		[[nodiscard]] constexpr ArrayConstIterator operator+(size_t x) const { return m_Ref + x; }
		[[nodiscard]] constexpr ArrayConstIterator operator-(size_t x) const { return m_Ref - x; }

		[[nodiscard]] constexpr T* operator->(void) const { return m_Ref.ptr(); }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ref; }

		[[nodiscard]] constexpr Ref<T> ref(void) { return m_Ref; }
		[[nodiscard]] constexpr T* ptr(void) { return m_Ref.ptr(); }

		[[nodiscard]] constexpr Reversed reverse(void) { return m_Ref; }

		[[nodiscard]] constexpr bool operator==(const ArrayConstIterator& other)  const { return m_Ref == other.m_Ref; }
		[[nodiscard]] constexpr auto operator<=>(const ArrayConstIterator& other) const { return m_Ref <=> other.m_Ref; }

		[[nodiscard]] static ArrayConstIterator _None(void) { return Ref<T>::_None(); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
	private:
	    Ref<T> m_Ref;
    };

    template<typename Array>
    class ArrayReverseIterator {
	public:
	    using T = Array::ValueType;
	    using ValueType = Array::ValueType;
		using ArrayType = Array;
		using Reversed = Array::Iterator;

        ArrayReverseIterator(Ref<T> ref) : m_Ref(ref) {}

		constexpr auto& operator++(void) { m_Ref--; return *this; }
		constexpr auto  operator++(int) { return ArrayReverseIterator(m_Ref--); }

		constexpr auto& operator--(void) { m_Ref++; return *this; }
		constexpr auto  operator--(int) { return ArrayReverseIterator(m_Ref++); }

		constexpr auto& operator+=(size_t x) { m_Ref -= x; return *this; }
		constexpr auto& operator-=(size_t x) { m_Ref += x; return *this; }

		[[nodiscard]] constexpr ArrayReverseIterator operator+(size_t x) const { return m_Ref - x; }
		[[nodiscard]] constexpr ArrayReverseIterator operator-(size_t x) const { return m_Ref + x; }

		[[nodiscard]] constexpr T* operator->(void) const { return m_Ref.ptr(); }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ref; }

		[[nodiscard]] constexpr Ref<T> ref(void) { return m_Ref; }
		[[nodiscard]] constexpr T* ptr(void) { return m_Ref.ptr(); }

		[[nodiscard]] constexpr Reversed reverse(void) { return m_Ref; }

		[[nodiscard]] constexpr bool operator==(const ArrayReverseIterator& other)  const { return m_Ref == other.m_Ref; }
		[[nodiscard]] constexpr auto operator<=>(const ArrayReverseIterator& other) const { return other.m_Ref <=> m_Ref; }

		[[nodiscard]] static ArrayReverseIterator _None(void) { return Ref<T>::_None(); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
	private:
	    Ref<T> m_Ref;
    };

    template<typename Array>
    class ArrayConstReverseIterator {
	public:
	    using T = const Array::ValueType;
	    using ValueType = const Array::ValueType;
		using ArrayType = Array;
		using Reversed = Array::ConstIterator;

        ArrayConstReverseIterator(Ref<T> ref) : m_Ref(ref) {}

		constexpr auto& operator++(void) { m_Ref--; return *this; }
		constexpr auto  operator++(int) { return ArrayConstReverseIterator(m_Ref--); }

		constexpr auto& operator--(void) { m_Ref++; return *this; }
		constexpr auto  operator--(int) { return ArrayConstReverseIterator(m_Ref++); }

		constexpr auto& operator+=(size_t x) { m_Ref -= x; return *this; }
		constexpr auto& operator-=(size_t x) { m_Ref += x; return *this; }

		[[nodiscard]] constexpr ArrayConstReverseIterator operator+(size_t x) const { return m_Ref - x; }
		[[nodiscard]] constexpr ArrayConstReverseIterator operator-(size_t x) const { return m_Ref + x; }

		[[nodiscard]] constexpr T* operator->(void) const { return m_Ref.ptr(); }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ref; }

		[[nodiscard]] constexpr Ref<T> ref(void) { return m_Ref; }
		[[nodiscard]] constexpr T* ptr(void) { return m_Ref.ptr(); }

		[[nodiscard]] constexpr Reversed reverse(void) { return m_Ref; }

		[[nodiscard]] constexpr bool operator==(const ArrayConstReverseIterator& other)  const { return m_Ref == other.m_Ref; }
		[[nodiscard]] constexpr auto operator<=>(const ArrayConstReverseIterator& other) const { return other.m_Ref <=> m_Ref; }

		[[nodiscard]] static ArrayConstReverseIterator _None(void) { return Ref<T>::_None(); }
		[[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
	private:
	    Ref<T> m_Ref;
    };
} // namespace Nrl
