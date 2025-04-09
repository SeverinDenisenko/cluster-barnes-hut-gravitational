#pragma once

#include "model.hpp"

namespace bh {

struct generator_params {
    u32 count;
    real scale_factor;
};

class generator {
public:
    generator(generator_params params);

    array<point_t> generate();

private:
    generator_params params_;
};

}
