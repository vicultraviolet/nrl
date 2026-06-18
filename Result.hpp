#pragma once

#include "./Option.hpp"
#include "./Ref.hpp"
#include "./Primitives.hpp"
#include "./Utils.hpp"
#include "./Tuple.hpp"

namespace Nrl {
    enum class ResultState : u8 {
        None, Ok, Err
    };

    template<typename T, typename U>
    union ResultData {
        ResultData(void) {}
        ~ResultData(void) {}

        T ok;
        U err;
    };

    template<typename T, typename... Args>
    struct OkArgs : public ArgsTuple_t<T, Args...> {};

    template<typename T>
    [[nodiscard]] constexpr OkArgs<RemoveReference_t<T>, T> Ok(T&& t) {
        return { ArgsTuple<RemoveReference_t<T>>(Forward<T>(t)) };
    }

    template<typename T, typename... Args>
    [[nodiscard]] constexpr OkArgs<T, Args...> MakeOk(Args&&... args) {
        return { ArgsTuple<T>(Forward<Args>(args)...) };
    }

    template<typename F, typename... Args>
    struct OkArgsWith : public ArgsTupleWith_t<F, Args...> {};

    template<typename F, typename... Args>
    [[nodiscard]] constexpr OkArgsWith<F, Args...> OkWith(F&& factory, Args&&... args) {
        return { ArgsTupleWith<F, Args...>(Forward<F>(factory), Forward<Args>(args)...) };
    }

    template<typename U, typename... Args>
    struct ErrArgs : public ArgsTuple_t<U, Args...> {};

    template<typename U>
    [[nodiscard]] constexpr ErrArgs<RemoveReference_t<U>, U> Err(U&& t) {
        return { ArgsTuple<RemoveReference_t<U>>(Forward<U>(t)) };
    }

    template<typename U, typename... Args>
    [[nodiscard]] constexpr ErrArgs<U, Args...> MakeErr(Args&&... args) {
        return { ArgsTuple<U>(Forward<Args>(args)...) };
    }

    template<typename F, typename... Args>
    struct ErrArgsWith : public ArgsTupleWith_t<F, Args...> {};

    template<typename F, typename... Args>
    [[nodiscard]] constexpr ErrArgsWith<F, Args...> ErrWith(F&& factory, Args&&... args) {
        return { ArgsTupleWith<F, Args...>(Forward<F>(factory), Forward<Args>(args)...) };
    }

    template<typename T, typename U>
    class Result {
        using ResultData = ResultData<T, U>;
    public:
        using AsRef = Result<Ref<T>, Ref<U>>;
        using AsConstRef = Result<Ref<const T>, Ref<const U>>;

        template<typename... Args>
        [[nodiscard]] static Result Ok(Args&&... args) {
            Result t;

            new (&t.m_Data.ok) T(Forward<Args>(args)...);
            t.m_State = ResultState::Ok;

            return t;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static Result OkWith(F&& factory, Args&&... args) {
            Result t;

            new (&t.m_Data.ok) T(factory(Forward<Args>(args)...));
            t.m_State = ResultState::Ok;

            return t;
        }

        template<c_HasMake Args>
        [[nodiscard]] static Result OkFrom(Args&& args) {
            Result t;

            new (&t.m_Data.ok) T(Forward<Args>(args).make());
            t.m_State = ResultState::Ok;

            return t;
        }

        template<typename... Args>
        [[nodiscard]] static Result Err(Args&&... args) {
            Result t;

            new (&t.m_Data.err) U(Forward<Args>(args)...);
            t.m_State = ResultState::Err;

            return t;
        }

        template<typename F, typename... Args>
        [[nodiscard]] static Result ErrWith(F&& factory, Args&&... args) {
            Result t;

            new(&t.m_Data.err) U(factory(Forward<Args>(args)...));
            t.m_State = ResultState::Err;

            return t;
        }

        template<c_HasMake Args>
        [[nodiscard]] static Result ErrFrom(Args&& args) {
            Result t;

            new (&t.m_Data.err) U(Forward<Args>(args).make());
            t.m_State = ResultState::Err;

            return t;
        }

        ~Result(void) { destroy(); }

        template<typename... Args>
        Result(const OkArgs<T, Args...>& args) {
            new (&m_Data.ok) T(args.make());
            m_State = ResultState::Ok;
        }
        template<typename... Args>
        Result& operator=(const OkArgs<T, Args...>& args) { return *this = Result(args); }

        template<typename... Args>
        Result(OkArgs<T, Args...>&& args) {
            new (&m_Data.ok) T(Move(args).make());
            m_State = ResultState::Ok;
        }
        template<typename... Args>
        Result& operator=(OkArgs<T, Args...>&& args) { return *this = Result(Move(args)); }

        template<typename F, typename... Args>
        Result(const OkArgsWith<F, Args...>& args) {
            new (&m_Data.ok) T(args.make());
            m_State = ResultState::Ok;
        }
        template<typename F, typename... Args>
        Result& operator=(const OkArgsWith<F, Args...>& args) { return *this = Result(args); }

        template<typename F, typename... Args>
        Result(OkArgsWith<F, Args...>&& args) {
            new (&m_Data.ok) T(Move(args).make());
            m_State = ResultState::Ok;
        }
        template<typename F, typename... Args>
        Result& operator=(OkArgsWith<F, Args...>&& args) { return *this = Result(Move(args)); }

        template<typename... Args>
        Result(const ErrArgs<U, Args...>& args) {
            new (&m_Data.err) U(args.make());
            m_State = ResultState::Err;
        }
        template<typename... Args>
        Result& operator=(const ErrArgs<U, Args...>& args) { return *this = Result(args); }

        template<typename... Args>
        Result(ErrArgs<U, Args...>&& args) {
            new (&m_Data.err) U(Move(args).make());
            m_State = ResultState::Err;
        }
        template<typename... Args>
        Result& operator=(ErrArgs<U, Args...>&& args) { return *this = Result(Move(args)); }

        template<typename F, typename... Args>
        Result(const ErrArgsWith<F, Args...>& args) {
            new (&m_Data.err) U(args.make());
            m_State = ResultState::Err;
        }
        template<typename F, typename... Args>
        Result& operator=(const ErrArgsWith<F, Args...>& args) { return *this = Result(args); }

        template<typename F, typename... Args>
        Result(ErrArgsWith<F, Args...>&& args) {
            new (&m_Data.err) U(Move(args).make());
            m_State = ResultState::Err;
        }
        template<typename F, typename... Args>
        Result& operator=(ErrArgsWith<F, Args...>&& args) { return *this = Result(Move(args)); }

        Result(const Result& other) {
            switch (other.m_State) {
                case ResultState::Ok:
                    new(&m_Data.ok) T(other.m_Data.ok);
                case ResultState::Err:
                    new(&m_Data.err) U(other.m_Data.err);
                default:
            }
            m_State = other.m_State;
        }

        Result& operator=(const Result& other) {
            if (this == &other)
                return *this;

            switch (other.m_State) {
                case ResultState::Ok:
                    switch (m_State) {
                        case ResultState::Ok:
                            m_Data.ok = other.m_Data.ok;
                        case ResultState::Err:
                            m_Data.err.~U();
                            new (&m_Data.ok) T(other.m_Data.ok);
                        default:
                            new (&m_Data.ok) T(other.m_Data.ok);
                }
                case ResultState::Err:
                    switch (m_State) {
                        case ResultState::Ok:
                            m_Data.ok.~T();
                            new (&m_Data.err) U(other.m_Data.err);
                        case ResultState::Err:
                            m_Data.err = other.m_Data.err;
                        default:
                            new (&m_Data.err) U(other.m_Data.err);
                    }
                default:
                    destroy();
            }
            m_State = other.m_State;

            return *this;
        }

        Result(Result&& other) noexcept {
            switch (other.m_State) {
                case ResultState::Ok:
                    new(&m_Data.ok) T(Move(other.m_Data.ok));
                case ResultState::Err:
                    new(&m_Data.err) U(Move(other.m_Data.err));
                default:
            }

            m_State = Exchange(other.m_State, ResultState::None);
        }

        Result& operator=(Result&& other) noexcept {
            if (this == &other)
                return *this;

            switch (other.m_State) {
                case ResultState::Ok:
                    switch (m_State) {
                        case ResultState::Ok:
                            m_Data.ok = Move(other.m_Data.ok);
                        case ResultState::Err:
                            m_Data.err.~U();
                            new (&m_Data.ok) T(Move(other.m_Data.ok));
                        default:
                            new (&m_Data.ok) T(Move(other.m_Data.ok));
                    }
                case ResultState::Err:
                    switch (m_State) {
                        case ResultState::Ok:
                            m_Data.ok.~T();
                            new (&m_Data.err) U(Move(other.m_Data.err));
                        case ResultState::Err:
                            m_Data.err = Move(other.m_Data.err);
                        default:
                            new (&m_Data.err) U(Move(other.m_Data.err));
                    }
                default:
                    destroy();
            }
            m_State = Exchange(other.m_State, ResultState::None);

            return *this;
        }

        [[nodiscard]] T unwrap(void) {
            NRL_ASSERT(is_ok(), "Could not unwrap Result: No ok value!");
            m_State = ResultState::None;
            return Move(m_Data.ok);
        }
        [[nodiscard]] U unwrap_err(void) {
            NRL_ASSERT(is_err(), "Could not unwrap Result: No err value!");
            m_State = ResultState::None;
            return Move(m_Data.err);
        }

        [[nodiscard]] Option<T> ok(void) {
            if (!is_ok())
                return None();

            m_State = ResultState::None;
            return Some<T>(Move(m_Data.ok));
        }
        [[nodiscard]] Option<U> err(void) {
            if (!is_err())
                return None();

            m_State = ResultState::None;
            return Some<U>(Move(m_Data.err));
        }

        void destroy() {
            if (is_ok())
                m_Data.ok.~T();
            else if (is_err())
                m_Data.err.~U();

            m_State = ResultState::None;
        }

        [[nodiscard]] AsRef ref(void) {
            switch (m_State) {
                case ResultState::Ok: return AsRef::OkWith(Ref<T>::New, m_Data.ok);
                case ResultState::Err: return AsRef::ErrWith(Ref<U>::New, m_Data.err);
                default: return AsRef::_None();
            }
        }

        [[nodiscard]] AsConstRef ref(void) const {
            switch (m_State) {
                case ResultState::Ok: return AsConstRef::OkWith(Ref<const T>::New, m_Data.ok);
                case ResultState::Err: return AsConstRef::ErrWith(Ref<const U>::New, m_Data.err);
                default: return AsRef::_None();
            }
        }

        [[nodiscard]] constexpr bool is_ok(void) const { return m_State == ResultState::Ok; }
        [[nodiscard]] constexpr bool is_err(void) const { return m_State == ResultState::Err; }

        [[nodiscard]] static Result _None(void) { return Result(); }
        [[nodiscard]] constexpr bool _is_none(void) const { return m_State == ResultState::None; }
    private:
        Result(void) = default;
    private:
        ResultData m_Data;
        ResultState m_State = ResultState::None;
    };
} // namespace Nrl
