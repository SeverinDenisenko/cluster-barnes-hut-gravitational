#pragma once

#include "model.hpp"

namespace bh {

struct generator_params {
    u32 count;
    real min_mass;
    real max_mass;
    real power_mass;
    real scale_mass;
    real max_distance;
    real scale_velocity;
};

class generator {
public:
    generator(generator_params params);

    array<point_t> generate();

private:
    generator_params params_;
};

}
