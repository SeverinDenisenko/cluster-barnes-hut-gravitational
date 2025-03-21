#pragma once

#include "linalg.hpp"
#include "model.hpp"
#include "types.hpp"

namespace bh {
class frontend {
public:
    frontend(array<point_t>& points);

    void open();

    void update();

    void close();

private:
    vec2 space_to_screen(vec2 position);
    real mass_to_radius(real mass, real min_mass, real max_mass);

    array<point_t>& points_;
    vec2 screen_size_;
    real scale_factor_;
};

}
