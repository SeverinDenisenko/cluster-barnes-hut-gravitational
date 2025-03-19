#pragma once

#include "model.hpp"

namespace bh {

struct generator_params {
    u32 count;
    real min_mass;
    real max_mass;
    real max_distance;
    real min_velocity;
    real max_velocity;
};

class generator {
public:
    generator(generator_params params);

    array<point_t> generate();

private:
    generator_params params_;
};

}
