#pragma once

#include "types.hpp"

namespace bh {

enum class cluster_messages : u32 {
    chunk         = 1,
    solver_params = 2,
    points        = 3,
};

}
