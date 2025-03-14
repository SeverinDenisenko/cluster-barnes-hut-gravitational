#pragma once

#include "types.hpp"

#include "vector.hpp"

namespace bh {

template <u32 Dim>
using vec = vector<real, Dim>;

using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;
using vec6 = vec<6>;

}
