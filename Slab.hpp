#pragma once

#include "./Ref.hpp"
#include "./ArrayIterator.hpp"
#include "./Subrange.hpp"

namespace Nrl {
    template<typename T, usize L>
    class Slab {
    public:
        using ValueType = T;
        using Iterator = ArrayIterator<Slab>;
        using ConstIterator = ArrayConstIterator<Slab>;
        using ReverseIterator = ArrayReverseIterator<Slab>;
        using ConstReverseIterator = ArrayConstReverseIterator<Slab>;
    public:
        template<typename... Args>
        [[nodiscard]] static Slab New(Args&&... args) {
            static_assert(sizeof...(Args) == L, "Failed to create Slab: Argument count mismatch!");

            Slab a;

            usize i = 0;
            ((void)[&](void) {
                new (a._ptr_at(i)) T(Forward<Args>(args));
                i++;
            }(), ...);

            return a;
        }

        template<c_HasMake... Args>
        [[nodiscard]] static Slab NewFrom(Args&&... args) {
            static_assert(sizeof...(Args) == L, "Failed to create Slab: Argument count mismatch!");

            Slab a;

            usize i = 0;
            ((void)[&](void) {
                new (a._ptr_at(i)) T(Forward<Args>(args).make());
                i++;
            }(), ...);

            return a;
        }

        template<c_InputIterator It>
        [[nodiscard]] static Slab Copy(Subrange<It> range) {
            NRL_ASSERT(range.length() == L, "Failed to create Slab: range size mismatch!");

            Slab a;

            for (usize i = 0; const T& x : range)
                new (a._ptr_at(i++)) T(x);

            return a;
        }
        template<c_InputIterator It>
        [[nodiscard]] static Slab Move(Subrange<It> range) {
            NRL_ASSERT(range.length() == L, "Failed to create Slab: range size mismatch!");

            Slab a;

            for (usize i = 0; const T& x : range)
                new (a._ptr_at(i++)) T(Nrl::Move(x));

            return a;
        }

        template<typename... Args>
        [[nodiscard]] static Slab Fill(const Args&... args) {
            Slab a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(args...);

            return a;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static Slab FillWith(F&& factory, const Args&... args) {
            Slab a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(factory(args...));

            return a;
        }

        template<typename Args>
        [[nodiscard]] static Slab FillFrom(Args&& args) {
            Slab a;

            for (auto it = a.begin(); it != a.end(); it++)
                new (it.ptr()) T(args.make());

            return a;
        }

        ~Slab(void) {
            for (auto it = rbegin(); it != rend(); it++)
                it->~T();
        }

        Slab(const Slab& other) {
            usize i = 0;
            for (auto it = begin(); it != end(); it++)
                new (it.ptr()) T(other[i++]);
        }
        Slab& operator=(const Slab& other) {
            if (this == &other)
                return *this;

            usize i = 0;
            for (auto it = begin(); it != end(); it++)
                *it = other[i++];

            return *this;
        }

        Slab(Slab&& other) noexcept {
            usize i = 0;
            for (auto it = begin(); it != end(); it++)
                new (it.ptr()) T(Nrl::Move(other[i++]));
        }
        Slab& operator=(Slab&& other) noexcept {
            if (this == &other)
                return *this;

            usize i = 0;
            for (auto it = begin(); it != end(); it++)
                *it = Nrl::Move(other[i++]);

            return *this;
        }

        [[nodiscard]] constexpr Iterator at(usize i) { return ref() + i; }
        [[nodiscard]] constexpr ConstIterator at(usize i) const { return ref() + i; }

        [[nodiscard]] constexpr ReverseIterator rat(usize i) { return at(L).reverse() + 1 + i; }
        [[nodiscard]] constexpr ConstReverseIterator rat(usize i) const { return at(L).reverse() + 1 + i; }

        [[nodiscard]] constexpr Iterator begin(void) { return at(0); }
        [[nodiscard]] constexpr Iterator end(void) { return at(L); }

        [[nodiscard]] constexpr ConstIterator begin(void) const { return at(0); }
        [[nodiscard]] constexpr ConstIterator end(void) const { return at(L); }

        [[nodiscard]] constexpr ReverseIterator rbegin(void) { return rat(0); }
        [[nodiscard]] constexpr ReverseIterator rend(void) { return rat(L); }

        [[nodiscard]] constexpr ConstReverseIterator rbegin(void) const { return rat(0); }
        [[nodiscard]] constexpr ConstReverseIterator rend(void) const { return rat(L); }

        [[nodiscard]] constexpr T& operator[](usize i) { return *(ref() + i); }
        [[nodiscard]] constexpr const T& operator[](usize i) const { return *(ref() + i); }

        [[nodiscard]] constexpr Ref<T> ref(void) { return RefFromPtr((T*)m_Data); }
        [[nodiscard]] constexpr Ref<const T> ref(void) const { return RefFromPtr((const T*)m_Data); }

        [[nodiscard]] constexpr usize length(void) const { return L; }
        [[nodiscard]] constexpr usize capacity(void) const { return L; }

        [[nodiscard]] constexpr usize size(void) const { return L * sizeof(T); }
        [[nodiscard]] constexpr usize max_size(void) const { return L * sizeof(T); }
    private:
        [[nodiscard]] constexpr T* _ptr_at(usize i) { return (ref() + i).ptr(); }
        [[nodiscard]] constexpr const T* _ptr_at(usize i) const { return (ref() + i).ptr(); }

        Slab(void) = default;
    private:
        alignas(T) ubyte m_Data[L * sizeof(T)];
    };

    template<typename T, typename... Args>
    [[nodiscard]] Slab<T, sizeof...(Args)> NewSlab(Args&&... args) {
        return Slab<T, sizeof...(Args)>::New(Forward<Args>(args)...);
    }

    template<c_HasMake First, c_HasMake... Args>
    [[nodiscard]] auto NewSlabFrom(First&& first, Args&&... args) {
        return Slab<typename First::Type, sizeof...(Args)+1>::NewFrom(Forward<First>(first), Forward<Args>(args)...);
    }

    template<typename T, usize L, typename... Args>
    [[nodiscard]] Slab<T, L> FillSlab(const Args&... args) {
        return Slab<T, L>::Fill(args...);
    }

    template<usize L, typename F, typename... Args>
    [[nodiscard]] auto FillSlabWith(F&& factory, const Args&... args) {
        return Slab<InvokeResult_t<F, Args...>, L>::Fill(Forward<F>(factory), args...);
    }

    template<usize L, typename Args>
    [[nodiscard]] auto FillSlabFrom(Args&& args) {
        return Slab<typename Args::Type, L>::FillFrom(Forward<Args>(args));
    }
} // namespace Nrl
