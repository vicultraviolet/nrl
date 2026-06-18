#pragma once

#include "./Traits.hpp"
#include "./Utils.hpp"
#include "./Sequence.hpp"

namespace Nrl {
    template<typename... Signature>
    class Tuple;

    template<>
    class Tuple<> {
    public:
        Tuple(void) = default;
        Tuple(InPlaceTag) {}

        [[nodiscard]] constexpr static Tuple Default(void) { return Tuple(); }

        [[nodiscard]] constexpr bool operator==(const Tuple& other) const { return true; }
        [[nodiscard]] constexpr bool operator!=(const Tuple& other) const { return false; }
    };

    template<typename Head, typename... Tail>
    class Tuple<Head, Tail...> : private Tuple<Tail...> {
        using Base = Tuple<Tail...>;
    public:
        [[nodiscard]] static Tuple Default(void) { return Tuple(); }

        [[nodiscard]] static Tuple New(Head&& h, Tail&&... tail) { return Tuple(Forward<Head>(h), Forward<Tail>(tail)...); }

        template<typename HeadArgs, typename... TailArgs>
        [[nodiscard]] static Tuple NewFrom(HeadArgs&& head_args, TailArgs&&... tail_args) {
            return Tuple(k_InPlace, Forward<HeadArgs>(head_args), Forward<TailArgs>(tail_args)...);
        }

        ~Tuple(void) = default;

        Tuple(const Tuple& other) : Base(other), m_Head(other.m_Head) {}
        Tuple& operator=(const Tuple& other) {
            if (this == &other)
                return *this;

            Base::operator=(other);
            m_Head = other.m_Head;

            return *this;
        }

        Tuple(Tuple&& other) noexcept : Base(Move(other)), m_Head(Move(other.m_Head)) {}
        Tuple& operator=(Tuple&& other) noexcept {
            if (this == &other)
                return *this;

            Base::operator=(Move(other));
            m_Head = Move(other.m_Head);

            return *this;
        }

        [[nodiscard]] constexpr Base& tail(void) { return (Base&)*this; }
        [[nodiscard]] constexpr const Base& tail(void) const { return (const Base&)*this; }

        [[nodiscard]] constexpr Head& head(void) { return m_Head; }
        [[nodiscard]] constexpr const Head& head(void) const { return m_Head; }

        [[nodiscard]] constexpr bool operator==(const Tuple& other) const { return m_Head == other.m_Head && tail() == other.tail(); }
        [[nodiscard]] constexpr bool operator!=(const Tuple& other) const { return !(*this == other); }
    protected:
        Tuple(Head&& h, Tail&&... tail)
        : Base(Forward<Tail>(tail)...),
          m_Head(Forward<Head>(h))
        {}

        template<typename HeadArgs, typename... TailArgs>
        Tuple(InPlaceTag, HeadArgs&& head_args, TailArgs&&... tail_args)
        : Base(k_InPlace, Forward<TailArgs>(tail_args)...),
          m_Head(Forward<HeadArgs>(head_args).make())
        {}
    private:
        Head m_Head;
    };

    template<typename T>
    struct TupleLength;

    template<typename... Signature>
    struct TupleLength<Tuple<Signature...>>
        : IntegralConstant<size_t, sizeof...(Signature)> {};

    template<typename... Signature>
    constexpr size_t TupleLength_v = TupleLength<Tuple<Signature...>>::value;

    template<size_t I, typename T>
    struct TupleElement;

    template<typename Head, typename... Tail>
    struct TupleElement<0, Tuple<Head, Tail...>> {
        using Type = Head;
    };

    template<size_t I, typename Head, typename... Tail>
    struct TupleElement<I, Tuple<Head, Tail...>> {
        using Type = typename TupleElement<I - 1, Tuple<Tail...>>::Type;
    };

    template<size_t I, typename T>
    using TupleElement_t = typename TupleElement<I, T>::Type;

    namespace Details {
        template<size_t I, typename... Signature>
        struct GetImpl;

        template<typename Head, typename... Tail>
        struct GetImpl<0, Head, Tail...> {
            [[nodiscard]] constexpr static Head& Apply(Tuple<Head, Tail...>& t) { return t.head(); }
            [[nodiscard]] constexpr static const Head& Apply(const Tuple<Head, Tail...>& t) { return t.head(); }
        };

        template<size_t I, typename Head, typename... Tail>
        struct GetImpl<I, Head, Tail...> {
            [[nodiscard]] constexpr static auto& Apply(Tuple<Head, Tail...>& t) {
                return GetImpl<I - 1, Tail...>::Apply(t.tail());
            }
            [[nodiscard]] constexpr static const auto& Apply(const Tuple<Head, Tail...>& t) {
                return GetImpl<I - 1, Tail...>::Apply(t.tail());
            }
        };
    } // namespace Details

    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr auto& Get(Tuple<Signature...>& t) {
        static_assert(I < sizeof...(Signature), "Failed to get element from tuple: Out of bounds!");
        return Details::GetImpl<I, Signature...>::Apply(t);
    }

    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr const auto& Get(const Tuple<Signature...>& t) {
        static_assert(I < sizeof...(Signature), "Failed to get element from tuple: Out of bounds!");
        return Details::GetImpl<I, Signature...>::Apply(t);
    }

    template<size_t I, typename... Signature>
    [[nodiscard]] constexpr auto&& Get(Tuple<Signature...>&& t) { return Move(Get<I>(t)); }

    namespace Details {
        template<typename T, typename Tuple, size_t... I>
        [[nodiscard]] constexpr T NewFromTupleImpl(Tuple&& t, IndexSequence<I...>) {
            return T(Get<I>(Forward<Tuple>(t))...);
        }

        template<typename T, typename F, typename Tuple, size_t... I>
        [[nodiscard]] constexpr T CallWithTupleImpl(F&& f, Tuple&& t, IndexSequence<I...>) {
            return f(Get<I>(Forward<Tuple>(t))...);
        }
    } // namespace Details

    template<typename T, typename... Args>
    [[nodiscard]] constexpr T NewFromTuple(const Tuple<Args...>& t) {
        return Details::NewFromTupleImpl<T>(t, NewIndexSequence<sizeof...(Args)>{});
    }
    template<typename T, typename... Args>
    [[nodiscard]] constexpr T NewFromTuple(Tuple<Args...>& t) {
        return Details::NewFromTupleImpl<T>(t, NewIndexSequence<sizeof...(Args)>{});
    }
    template<typename T, typename... Args>
    [[nodiscard]] constexpr T NewFromTuple(Tuple<Args...>&& t) {
        return Details::NewFromTupleImpl<T>(Move(t), NewIndexSequence<sizeof...(Args)>{});
    }

    template<typename F, typename... Args>
    constexpr auto CallWithTuple(F&& f, const Tuple<Args...>& t) {
        using T = InvokeResult_t<F, Args...>;
        return Details::CallWithTupleImpl<T, F>(Forward<F>(f), t, NewIndexSequence<sizeof...(Args)>{});
    }
    template<typename F, typename... Args>
    constexpr auto CallWithTuple(F&& f, Tuple<Args...>& t) {
        using T = InvokeResult_t<F, Args...>;
        return Details::CallWithTupleImpl<T, F>(Forward<F>(f), t, NewIndexSequence<sizeof...(Args)>{});
    }
    template<typename F, typename... Args>
    constexpr auto CallWithTuple(F&& f, Tuple<Args...>&& t) {
        using T = InvokeResult_t<F, Args...>;
        return Details::CallWithTupleImpl<T, F>(Forward<F>(f), Move(t), NewIndexSequence<sizeof...(Args)>{});
    }

    template<typename T, typename... Args>
    struct ArgsTuple_t {
        using Type = T;

        Tuple<Args...> tuple;

        template<typename F>
        constexpr T call(F&& f) const & { return CallWithTuple(Forward<F>(f), tuple); }
        template<typename F>
        constexpr T call(F&& f) & { return CallWithTuple(Forward<F>(f), tuple); }
        template<typename F>
        constexpr T call(F&& f) && { return CallWithTuple(Forward<F>(f), Move(tuple)); }

        [[nodiscard]] constexpr T make(void) const & { return NewFromTuple<T>(tuple); }
        [[nodiscard]] constexpr T make(void) & { return NewFromTuple<T>(tuple); }
        [[nodiscard]] constexpr T make(void) && { return NewFromTuple<T>(Move(tuple)); }

        [[nodiscard]] constexpr T operator()(void) const & { return make(); }
        [[nodiscard]] constexpr T operator()(void) & { return make(); }
        [[nodiscard]] constexpr T operator()(void) && { return make(); }
    };

    template<typename T, typename... Args>
    [[nodiscard]] ArgsTuple_t<T, Args...> ArgsTuple(Args&&... args) {
        return { Tuple<Args...>::New(Forward<Args>(args)...) };
    }

    template<typename F, typename... Args>
    struct ArgsTupleWith_t {
        using Type = InvokeResult_t<F, Args...>;
        using Function = F;

        F f;
        Tuple<Args...> tuple;

        constexpr Type call(void) const & { return CallWithTuple(f, tuple); }
        constexpr Type call(void) & { return CallWithTuple(f, tuple); }
        constexpr Type call(void) && { return CallWithTuple(Move(f), Move(tuple)); }

        [[nodiscard]] constexpr Type make(void) const & { return call(); }
        [[nodiscard]] constexpr Type make(void) & { return call(); }
        [[nodiscard]] constexpr Type make(void) && { return call(); }

        constexpr Type operator()(void) const & { return call(); }
        constexpr Type operator()(void) & { return call(); }
        constexpr Type operator()(void) && { return call(); }
    };

    template<typename F, typename... Args>
    [[nodiscard]] ArgsTupleWith_t<F, Args...> ArgsTupleWith(F&& f, Args&&... args) {
        return { Forward<F>(f), Tuple<Args...>::New(Forward<Args>(args)...) };
    }

    template<typename... Signature>
    [[nodiscard]] Tuple<Signature...> NewTuple(Signature&&... args) { return Tuple<Signature...>::New(Forward<Signature>(args)...); }

    template<typename... Args>
    [[nodiscard]] auto NewTupleFrom(Args&&... args) {
        return Tuple<typename Args::Type...>::NewFrom(Forward<Args>(args)...);
    }
} // namespace Nrl
