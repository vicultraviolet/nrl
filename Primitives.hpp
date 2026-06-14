#pragma once

#include <float.h>
#include <stdint.h>

namespace Nrl {
    namespace Primitives {
        using i8  = int8_t;
        using i16 = int16_t;
        using i32 = int32_t;
        using i64 = int64_t;

        constexpr i8  i8max  = INT8_MAX;
        constexpr i16 i16max = INT16_MAX;
        constexpr i32 i32max = INT32_MAX;
        constexpr i64 i64max = INT64_MAX;

        using u8  = uint8_t;
        using u16 = uint16_t;
        using u32 = uint32_t;
        using u64 = uint64_t;

        constexpr u8  u8max  = UINT8_MAX;
        constexpr u16 u16max = UINT16_MAX;
        constexpr u32 u32max = UINT32_MAX;
        constexpr u64 u64max = UINT64_MAX;

        using f32 = float;
        using f64 = double;

        constexpr f32 f32max = FLT_MAX;
        constexpr f64 f64max = DBL_MAX;

        using b8 = i8;
        using b32 = i32;

        constexpr b8 k_False = 0;
        constexpr b8 k_True  = 1;

        using byte_t = unsigned char;
    } // namespace Primitives
    using namespace Primitives;
} // namespace Nrl
