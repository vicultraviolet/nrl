#pragma once

#include "./Ref.hpp"
#include "./ArrayIterator.hpp"

namespace Nrl {
    template<typename T, size_t C>
    class BoundedArray {
    public:
        using ValueType = T;
        using Iterator = ArrayIterator<BoundedArray>;
        using ConstIterator = ArrayConstIterator<BoundedArray>;
        using ReverseIterator = ArrayReverseIterator<BoundedArray>;
        using ConstReverseIterator = ArrayConstReverseIterator<BoundedArray>;
    public:
        [[nodiscard]] static BoundedArray Default(void) { return BoundedArray(); }

        template<typename... Args>
        [[nodiscard]] static BoundedArray New(Args&&... args) {
            BoundedArray a;

            ((void)[&](void) {
                a.emplace(Forward<Args>(args));
            }(), ...);

            return a;
        }

        template<c_HasMake... Args>
        [[nodiscard]] static BoundedArray NewFrom(Args&&... args) {
            BoundedArray a;

            ((void)[&](void) {
                a.emplace(Forward<Args>(args));
            }(), ...);

            return a;
        }

        template<typename... Args>
        [[nodiscard]] static BoundedArray Fill(size_t length, const Args&... args) {
            BoundedArray a;

            while (a.m_Length != length)
                a.emplace(args...);

            return a;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static BoundedArray FillWith(size_t length, F&& factory, const Args&... args) {
            BoundedArray a;

            while (a.m_Length != length)
                a.emplace_with(Forward<F>(factory), args...);

            return a;
        }

        template<typename Args>
        [[nodiscard]] static BoundedArray FillFrom(size_t length, Args&& args) {
            BoundedArray a;

            while (a.m_Length != length)
                a.emplace_from(Forward<Args>(args));

            return a;
        }

        ~BoundedArray(void) { clear(); }

        BoundedArray(const BoundedArray& other) {
            while (m_Length != other.m_Length)
                emplace(other[m_Length]);
        }
        BoundedArray& operator=(const BoundedArray& other) {
            if (this == &other)
                return *this;

            if (m_Length == other.m_Length) {
                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = other[i++];
            } else
            if (m_Length > other.m_Length) {
                while (m_Length != other.m_Length)
                    pop();

                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = other[i++];
            } else
            if (m_Length < other.m_Length) {
                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = other[i++];

                while (m_Length != other.m_Length)
                    emplace(other[m_Length]);
            }

            return *this;
        }

        BoundedArray(BoundedArray&& other) noexcept {
            while (m_Length != other.m_Length)
                emplace(Move(other[m_Length]));
        }
        BoundedArray& operator=(BoundedArray&& other) noexcept {
            if (this == &other)
                return *this;

            if (m_Length == other.m_Length) {
                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = Move(other[i++]);
            } else
            if (m_Length > other.m_Length) {
                while (m_Length != other.m_Length)
                    pop();

                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = Move(other[i++]);
            } else
            if (m_Length < other.m_Length) {
                size_t i = 0;
                for (auto it = begin(); it != end(); it++)
                    *it = Move(other[i++]);

                while (m_Length != other.m_Length)
                    emplace(Move(other[m_Length]));
            }

            return *this;
        }

        template<typename... Args>
        size_t emplace(Args&&... args) {
            new (_ptr_at(m_Length)) T(Forward<Args>(args)...);
            return m_Length++;
        }

        template<typename F, typename... Args>
        size_t emplace_with(F&& factory, Args&&... args) {
            new (_ptr_at(m_Length)) T(factory(Forward<Args>(args)...));
            return m_Length++;
        }

        template<c_HasMake Args>
        size_t emplace_from(Args&& args) {
            new (_ptr_at(m_Length)) T(Forward<Args>(args).make());
            return m_Length++;
        }

        void pop(void) {
            _ptr_at(--m_Length)->~T();
        }

        void clear(void) {
            while (m_Length != 0)
                pop();
        }

        [[nodiscard]] constexpr Iterator at(size_t i) { return ref() + i; }
        [[nodiscard]] constexpr ConstIterator at(size_t i) const { return ref() + i; }

        [[nodiscard]] constexpr ReverseIterator rat(size_t i) { return at(m_Length - 1).reverse() + i; }
        [[nodiscard]] constexpr ConstReverseIterator rat(size_t i) const { return at(m_Length - 1).reverse() + i; }

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

        [[nodiscard]] constexpr Ref<T> ref(void) { return RefFromPtr((T*)m_Data); }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return RefFromPtr((const T*)m_Data); }

        [[nodiscard]] constexpr size_t length(void) const { return m_Length; }
        [[nodiscard]] constexpr size_t capacity(void) const { return C; }

        [[nodiscard]] constexpr size_t size(void) const { return m_Length * sizeof(T); }
        [[nodiscard]] constexpr size_t max_size(void) const { return C * sizeof(T); }
    private:
        [[nodiscard]] constexpr T* _ptr_at(size_t i) { return (ref() + i).ptr(); }
        [[nodiscard]] constexpr const T* _ptr_at(size_t i) const { return (ref() + i).ptr(); }

        BoundedArray(void) = default;
    private:
        alignas(T) ubyte m_Data[C * sizeof(T)];
        size_t m_Length = 0;
    };

    template<typename T, typename... Args>
    [[nodiscard]] BoundedArray<T, sizeof...(Args)> NewBoundedArray(Args&&... args) {
        return BoundedArray<T, sizeof...(Args)>::New(Forward<Args>(args)...);
    }

    template<c_HasMake First, c_HasMake... Args>
    [[nodiscard]] auto NewBoundedArrayFrom(First&& first, Args&&... args) {
        return BoundedArray<typename First::Type, sizeof...(Args)+1>::NewFrom(Forward<First>(first), Forward<Args>(args)...);
    }
} // namespace Nrl
