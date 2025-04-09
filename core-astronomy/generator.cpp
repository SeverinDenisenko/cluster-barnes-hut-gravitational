#include "generator.hpp"

#include <cmath>
#include <random>

namespace bh {

generator::generator(generator_params params)
    : params_(params)
{
}

array<point_t> generator::generate()
{
    array<point_t> points;

    std::random_device rand_dev;
    std::mt19937 rand_engine(rand_dev());
    std::uniform_real_distribution<real> angle_dist(0.0_r, 2.0_r * M_PI);
    std::uniform_real_distribution<real> enclosed_mass_dist(0.0_r, 1.0_r);
    std::uniform_real_distribution<real> position_distribution(-1.0_r, 1.0_r);
    std::uniform_real_distribution<real> uniform_distribution(0.0_r, 1.0_r);

    real speed_scale_factor = 1.0_r / params_.scale_factor;
    real mass               = 1.0_r / params_.count;

    for (u32 body = 0; body < params_.count; ++body) {
        real enclosed_mass    = enclosed_mass_dist(rand_engine);
        real enclosing_radius = 1.0_r / std::sqrt(std::pow(enclosed_mass, -2.0_r / 3.0_r) - 1.0_r);

        vec2 position;
        do {
            for (u32 i = 0; i < 2; i++) {
                position[i] = position_distribution(rand_engine);
            }
        } while (position.len() > 1.0);
        position = position * (enclosing_radius * params_.scale_factor / position.len());

        real x, y;
        do {
            x = uniform_distribution(rand_engine);
            y = uniform_distribution(rand_engine) / 10.0_r;
        } while (y > x * x * std::pow(1.0_r - x * x, 3.5_r));
        real speed_scale = x * std::sqrt(2.0_r / std::sqrt(1.0_r + enclosing_radius * enclosing_radius));

        vec2 velosity;
        do {
            for (u32 i = 0; i < 2; i++) {
                velosity[i] = position_distribution(rand_engine);
            }
        } while (velosity.len() > 1.0);
        velosity = velosity * (speed_scale_factor * speed_scale / velosity.len());

        point_t point { .position = position, .velosity = velosity, .mass = mass };

        points.push_back(point);
    }

    return points;
}

}
