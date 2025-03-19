#include "generator.hpp"

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
    std::uniform_real_distribution<real> distanse_dist(0.0_r, params_.max_distance);
    std::uniform_real_distribution<real> mass_dist(params_.min_mass, params_.max_mass);
    std::uniform_real_distribution<real> velocity_dist(params_.min_velocity, params_.max_velocity);

    for (u32 i = 0; i < params_.count; ++i) {
        real angle = angle_dist(rand_engine);

        vec2 position = vec2 { std::cos(angle), std::sin(angle) } * distanse_dist(rand_engine);
        vec2 velosity = vec2 { -std::sin(angle), std::cos(angle) } * velocity_dist(rand_engine);

        point_t point { .position     = position,
                        .velosity     = velosity,
                        .acceleration = vec2 { 0.0_r, 0.0_r },
                        .mass         = mass_dist(rand_engine) };

        points.push_back(point);
    }

    return points;
}

}
