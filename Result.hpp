#pragma once

#include "./Option.hpp"
#include "./Ref.hpp"
#include "./Primitives.hpp"
#include "./Utils.hpp"

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

        ~Result(void) { destroy(); }

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

    template<typename T, typename U, typename... Args>
    [[nodiscard]] Result<T, U> Ok(Args&&... args) {
        return Result<T, U>::Ok(Forward<Args>(args)... );
    }

    template<typename T, typename U, typename F, typename... Args>
    [[nodiscard]] Result<T, U> OkWith(F&& factory, Args&&... args) {
        return Result<T, U>::OkWith(Forward<F>(factory), Forward<Args>(args)...);
    }

    template<typename T, typename U, typename... Args>
    [[nodiscard]] Result<T, U> Err(Args&&... args) {
        return Result<T, U>::Err(Forward<Args>(args)... );
    }

    template<typename T, typename U, typename F, typename... Args>
    [[nodiscard]] Result<T, U> ErrWith(F&& factory, Args&&... args) {
        return Result<T, U>::ErrWith(Forward<F>(factory), Forward<Args>(args)...);
    }
} // namespace Nrl
