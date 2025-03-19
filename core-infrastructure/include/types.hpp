#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace bh {

using u8  = std::uint8_t;
using i8  = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;

using f64 = double;
using f32 = float;

using real = f64;

inline real operator""_r(long double d)
{
    return static_cast<real>(d);
}

template <typename T>
using array = std::vector<T>;

template <typename T, size_t Size>
using static_array = std::array<T, Size>;

}
