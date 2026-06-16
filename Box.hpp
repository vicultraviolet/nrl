#pragma once

#include "./Traits.hpp"
#include "./DefaultDeleter.hpp"
#include "./Downcast.hpp"

namespace Nrl {
    template<typename T, typename Deleter = DefaultDeleter<T>>
    class Box {
    public:
        [[nodiscard]] static Box New(Ref<T> ref) { return ref; }

        template<typename... Args>
            requires SameAs_v<Deleter, DefaultDeleter<T>>
        [[nodiscard]]
        static Box Make(Args&&... args) {
            auto allocator = Allocator<T>::New();
            Ref<T> ref = allocator.alloc(1);
            allocator.construct(ref, Forward<Args>(args)...);
            return ref;
        }

        template<typename F, typename... Args>
            requires SameAs_v<Deleter, DefaultDeleter<T>>
        [[nodiscard]]
        static Box MakeWith(F&& factory, Args&&... args) {
            auto allocator = Allocator<T>::New();
            Ref<T> ref = allocator.alloc(1);
            allocator.construct_with(ref, Forward<F>(factory), Forward<Args>(args)...);
            return ref;
        }

        ~Box(void) {
            if (m_Ref._is_some())
                Deleter::New()(m_Ref);
        }

        Box(const Box& other) = delete;
        Box& operator=(const Box& other) = delete;

        Box(Box&& other) : m_Ref(Move(other.m_Ref)) {}
        Box& operator=(Box&& other) {
            if (this == &other)
                return *this;

            if (m_Ref._is_some())
                Deleter::New()(m_Ref);

            m_Ref = Move(other.m_Ref);

            return *this;
        }

        [[nodiscard]] Ref<T> release(void) {
            NRL_ASSERT(_is_some(), "Could not release Box: Invalid Box!");
            Ref<T> temp = m_Ref;
            m_Ref = Ref<T>::_None();
            return temp;
        }

        [[nodiscard]] T take(void)
            requires SameAs_v<Deleter, DefaultDeleter<T>>
        {
            NRL_ASSERT(_is_some(), "Could not take value from Box: Invalid Box!");
            T t = Move(*m_Ref);
            auto allocator = Allocator<T>::New();
            allocator.dealloc(m_Ref, 1);
            m_Ref = Ref<T>::_None();
            return t;
        }

        [[nodiscard]] constexpr operator Ref<T>(void) const { return m_Ref; }
        [[nodiscard]] constexpr Ref<T> ref(void) const { return m_Ref; }

        template<typename U>
            requires IsBaseOf_v<U, T>
		[[nodiscard]] constexpr operator Box<U>(void) && { return Box<U>::New((Ref<U>)this->release()); }

        [[nodiscard]] constexpr T* ptr(void) const { return m_Ref.ptr(); }
		[[nodiscard]] constexpr T& operator*(void) const { return *m_Ref; }
		[[nodiscard]] constexpr T* operator->(void) const { return m_Ref.ptr(); }

		[[nodiscard]] constexpr auto operator<=>(const Box& other) const { return m_Ref <=> other.m_Ref; }
		[[nodiscard]] constexpr auto operator==(const Box& other) const { return m_Ref == other.m_Ref; }

        [[nodiscard]] static Box _None(void) { return Ref<T>::_None(); }
        [[nodiscard]] constexpr bool _is_some(void) const { return m_Ref._is_some(); }
    private:
        Box(Ref<T> ref) : m_Ref(ref) {}

		template<typename To, typename From>
		friend Box<To> DynamicCast(Box<From>&& from);
    private:
        Ref<T> m_Ref;
    };

    template<typename To, typename From>
    [[nodiscard]]
    Option<Box<To>> Downcast(Box<From>&& from) {
        Option<Ref<To>> to = Downcast<To, From>(from.ref());
        if (to.is_none())
            return None();

        (void)from.release();
        return SomeWith<Box<To>>(Box<To>::New, to.unwrap());
	}
} // namespace Nrl
