#pragma once

#include "./Primitives.hpp"
#include "./Ref.hpp"
#include "./ArrayIterator.hpp"

namespace Nrl {
    template<typename T, size_t L>
    class Array {
    public:
        using ValueType = T;
        using Iterator = ArrayIterator<Array>;
        using ConstIterator = ArrayConstIterator<Array>;
        using ReverseIterator = ArrayReverseIterator<Array>;
        using ConstReverseIterator = ArrayConstReverseIterator<Array>;
    public:
        template<typename... Args>
        [[nodiscard]] static Array New(Args&&... args) {
            Array a;

            size_t i = 0;
            ((void)[&](void) {
                new (a._ptr_at(i)) T(Forward<Args>(args));
                i++;
            }(), ...);

            return a;
        }

        template<c_HasMake... Args>
        [[nodiscard]] static Array NewFrom(Args&&... args) {
            Array a;

            size_t i = 0;
            ((void)[&](void) {
                new (a._ptr_at(i)) T(Forward<Args>(args).make());
                i++;
            }(), ...);

            return a;
        }

        template<typename... Args>
        [[nodiscard]] static Array Fill(const Args&... args) {
            Array a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(args...);

            return a;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static Array FillWith(F&& factory, const Args&... args) {
            Array a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(factory(args...));

            return a;
        }

        template<typename Args>
        [[nodiscard]] static Array FillFrom(Args&& args) {
            Array a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(args.make());

            return a;
        }

        ~Array(void) {
            for (auto it = rbegin(); it != rend(); it++)
                it->~T();
        }

        Array(const Array& other) {
            size_t i = 0;
            for (auto it = begin(); it != end(); it++)
                new (it.ptr()) T(other[i++]);
        }
        Array& operator=(const Array& other) {
            if (this == &other)
                return *this;

            size_t i = 0;
            for (auto it = begin(); it != end(); it++)
                *it = other[i++];

            return *this;
        }

        Array(Array&& other) noexcept {
            size_t i = 0;
            for (auto it = begin(); it != end(); it++)
                new (it.ptr()) T(Move(other[i++]));
        }
        Array& operator=(Array&& other) noexcept {
            if (this == &other)
                return *this;

            size_t i = 0;
            for (auto it = begin(); it != end(); it++)
                *it = Move(other[i++]);

            return *this;
        }

        [[nodiscard]] constexpr Iterator at(size_t i) { return ref() + i; }
        [[nodiscard]] constexpr ConstIterator at(size_t i) const { return ref() + i; }

        [[nodiscard]] constexpr ReverseIterator rat(size_t i) { return at(L - 1).reverse() + i; }
        [[nodiscard]] constexpr ConstReverseIterator rat(size_t i) const { return at(L - 1).reverse() + i; }

        [[nodiscard]] constexpr Iterator begin(void) { return at(0); }
        [[nodiscard]] constexpr Iterator end(void) { return at(L); }

        [[nodiscard]] constexpr ConstIterator begin(void) const { return at(0); }
        [[nodiscard]] constexpr ConstIterator end(void) const { return at(L); }

        [[nodiscard]] constexpr ReverseIterator rbegin(void) { return rat(0); }
        [[nodiscard]] constexpr ReverseIterator rend(void) { return rat(L); }

        [[nodiscard]] constexpr ConstReverseIterator rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr ConstReverseIterator rend(void) const { return rat(L); }

        [[nodiscard]] constexpr T& operator[](size_t i) { return *(ref() + i); }
        [[nodiscard]] constexpr const T& operator[](size_t i) const { return *(ref() + i); }

        [[nodiscard]] constexpr Ref<T> ref(void) { return RefFromPtr((T*)m_Data); }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return RefFromPtr((const T*)m_Data); }

        [[nodiscard]] constexpr size_t length(void) const { return L; }
        [[nodiscard]] constexpr size_t capacity(void) const { return L; }

        [[nodiscard]] constexpr size_t size(void) const { return L * sizeof(T); }
        [[nodiscard]] constexpr size_t max_size(void) const { return L * sizeof(T); }
    private:
        [[nodiscard]] constexpr T* _ptr_at(size_t i) { return (ref() + i).ptr(); }
        [[nodiscard]] constexpr const T* _ptr_at(size_t i) const { return (ref() + i).ptr(); }

        Array(void) = default;
    private:
        alignas(T) ubyte m_Data[L * sizeof(T)];
    };

    template<typename T, typename... Args>
    [[nodiscard]] Array<T, sizeof...(Args)> NewArray(Args&&... args) {
        return Array<T, sizeof...(Args)>::New(Forward<Args>(args)...);
    }

    template<c_HasMake First, c_HasMake... Args>
    [[nodiscard]] auto NewArrayFrom(First&& first, Args&&... args) {
        return Array<typename First::Type, sizeof...(Args)+1>::NewFrom(Forward<First>(first), Forward<Args>(args)...);
    }

    template<size_t L, typename F, typename... Args>
    [[nodiscard]] auto FillArrayWith(F&& factory, const Args&... args) {
        return Array<InvokeResult_t<F, Args...>, L>::Fill(Forward<F>(factory), args...);
    }

    template<size_t L, typename Args>
    [[nodiscard]] auto FillArrayFrom(Args&& args) {
        return Array<typename Args::Type, L>::FillFrom(Forward<Args>(args));
    }
} // namespace Nrl
