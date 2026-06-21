#pragma once

#include "./AlignedHeapAllocator.hpp"
#include "./ArrayIterator.hpp"

namespace Nrl {
    template<typename T>
    class DynamicArray {
    public:
        using ValueType = T;
        using Iterator = ArrayIterator<DynamicArray>;
        using ConstIterator = ArrayConstIterator<DynamicArray>;
        using ReverseIterator = ArrayReverseIterator<DynamicArray>;
        using ConstReverseIterator = ArrayConstReverseIterator<DynamicArray>;
    public:
        [[nodiscard]] static DynamicArray Empty(void) { return DynamicArray(); }

        template<typename... Args>
        [[nodiscard]] static DynamicArray New(Args&&... args) {
            DynamicArray a;

            ((void)[&](void) {
                a.emplace(Forward<Args>(args));
            }(), ...);

            return a;
        }

        template<c_HasMake... Args>
        [[nodiscard]] static DynamicArray NewFrom(Args&&... args) {
            DynamicArray a;

            ((void)[&](void) {
                a.emplace_from(Forward<Args>(args));
            }(), ...);

            return a;
        }

        template<typename... Args>
        [[nodiscard]] static DynamicArray Fill(size_t length, const Args&... args) {
            DynamicArray a;

            while (a.m_Length != length)
                a.emplace(args...);

            return a;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static DynamicArray FillWith(size_t length, F&& factory, const Args&... args) {
            DynamicArray a;

            while (a.m_Length != length)
                a.emplace_with(Forward<F>(factory), args...);

            return a;
        }

        template<typename Args>
        [[nodiscard]] static DynamicArray FillFrom(size_t length, Args&& args) {
            DynamicArray a;

            while (a.m_Length != length)
                a.emplace_from(Forward<Args>(args));

            return a;
        }

        ~DynamicArray(void) { dispose(); }

        DynamicArray(const DynamicArray& other) {
            reserve(other.m_Capacity);
            while (m_Length != other.m_Length)
                emplace(other[m_Length]);
        }
        DynamicArray& operator=(const DynamicArray& other) {
            if (this == &other)
                return *this;

            if (m_Capacity != other.m_Capacity) {
                dispose();
                reserve(other.m_Capacity);
            } else {
                clear();
            }

            while (m_Length != other.m_Length)
                emplace(other[m_Length]);

            return *this;
        }

        DynamicArray(DynamicArray&& other)
        : m_Data(Move(other.m_Data)),
          m_Length(Exchange(other.m_Length, 0)),
          m_Capacity(Exchange(other.m_Capacity, 0))
        {}
        DynamicArray& operator=(DynamicArray&& other) {
            if (this == &other)
                return *this;

            dispose();

            m_Data = Move(other.m_Data);
            m_Length = Exchange(other.m_Length, 0);
            m_Capacity = Exchange(other.m_Capacity, 0);

            return *this;
        }

        template<typename... Args>
        size_t emplace(Args&&... args) {
            _grow_if();
            m_Allocator.construct(m_Data + m_Length, Forward<Args>(args)...);
            return m_Length++;
        }

        template<typename F, typename... Args>
        size_t emplace_with(F&& factory, Args&&... args) {
            _grow_if();
            m_Allocator.construct_with(m_Data + m_Length, Forward<F>(factory), Forward<Args>(args)...);
            return m_Length++;
        }

        template<c_HasMake Args>
        size_t emplace_from(Args&& args) {
            _grow_if();
            m_Allocator.construct_from(m_Data + m_Length, Forward<Args>(args));
            return m_Length++;
        }

        void pop(void) {
            m_Allocator.destruct(m_Data + --m_Length);
        }
        void clear(void) {
            while (m_Length != 0)
                pop();
        }

        void reserve(size_t new_capacity) {
            NRL_ASSERT(n > m_Length, "Could not reserve memory for DynamicArray: new capacity is less than current length!");
            if (new_capacity == m_Capacity)
                return;

            Ref<T> new_data = m_Allocator.alloc(new_capacity);
            for (size_t i = 0; i < m_Length; i++)
                m_Allocator.construct(new_data + i, Move(*_ptr_at(i)));

            m_Allocator.dealloc(m_Data, m_Capacity);
            m_Capacity = new_capacity;
            m_Data = new_data;
        }

        void grow(size_t n) {
            reserve(m_Capacity + n);
        }
        void grow_by(float r) {
            reserve((size_t)(m_Capacity * r + 0.5f));
        }

        void dispose(void) {
            clear();
            m_Allocator.dealloc(m_Data, m_Capacity);
            m_Capacity = 0;
        }

        [[nodiscard]] constexpr Iterator at(size_t i) { return ref() + i; }
        [[nodiscard]] constexpr ConstIterator at(size_t i) const { return ref() + i; }

        [[nodiscard]] constexpr ReverseIterator rat(size_t i) { return at(m_Length).reverse() + 1 + i; }
        [[nodiscard]] constexpr ConstReverseIterator rat(size_t i) const { return at(m_Length).reverse() + 1 + i; }

        [[nodiscard]] constexpr Iterator begin(void) { return at(0); }
        [[nodiscard]] constexpr Iterator end(void) { return at(m_Length); }

        [[nodiscard]] constexpr ConstIterator begin(void) const { return at(0); }
        [[nodiscard]] constexpr ConstIterator end(void) const { return at(m_Length); }

        [[nodiscard]] constexpr ReverseIterator rbegin(void) { return rat(0); }
        [[nodiscard]] constexpr ReverseIterator rend(void) { return rat(m_Length); }

        [[nodiscard]] constexpr ConstReverseIterator rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr ConstReverseIterator rend(void) const { return rat(m_Length); }

        [[nodiscard]] constexpr T& operator[](size_t i) { return *(ref() + i); }
        [[nodiscard]] constexpr const T& operator[](size_t i) const { return *(ref() + i); }

        [[nodiscard]] constexpr Ref<T> ref(void) { return m_Data; }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return m_Data; }

        [[nodiscard]] constexpr size_t length(void) const { return m_Length; }
        [[nodiscard]] constexpr size_t capacity(void) const { return m_Capacity; }

        [[nodiscard]] constexpr size_t size(void) const { return m_Length * sizeof(T); }
        [[nodiscard]] constexpr size_t max_size(void) const { return m_Capacity * sizeof(T); }
    private:
        [[nodiscard]] constexpr T* _ptr_at(size_t i) { return (ref() + i).ptr(); }
        [[nodiscard]] constexpr const T* _ptr_at(size_t i) const { return (ref() + i).ptr(); }

        void _grow_if(void) {
            if (m_Length == m_Capacity) {
                if (m_Capacity == 0)
                    reserve(4);
                else
                    grow_by(2.0f);
            }
        }

        DynamicArray(void) = default;
    private:
        Ref<T> m_Data = Ref<T>::_None();
        size_t m_Length = 0, m_Capacity = 0;
        NRL_NO_UNIQUE_ADDRESS Allocator<T> m_Allocator = Allocator<T>::New();
    };

    template<typename T, typename... Args>
    [[nodiscard]] DynamicArray<T> NewDynamicArray(Args&&... args) {
        return DynamicArray<T>::New(Forward<Args>(args)...);
    }

    template<c_HasMake First, c_HasMake... Args>
    [[nodiscard]] auto NewDynamicArrayFrom(First&& first, Args&&... args) {
        return DynamicArray<typename First::Type>::NewFrom(Forward<First>(first), Forward<Args>(args)...);
    }
} // namespace Nrl
