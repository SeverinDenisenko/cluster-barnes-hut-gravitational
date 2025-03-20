#include "generator.hpp"

#include <cmath>
#include <random>

namespace bh {

generator::generator(generator_params params)
    : params_(params)
{
}

static real power_distribution(real min, real max, real power)
{
    static std::random_device rand_dev;
    static std::mt19937 rand_engine(rand_dev());
    static std::uniform_real_distribution<real> uniform(0.0_r, 1.0_r);

    return std::pow(
        (std::pow(max, power + 1) - std::pow(min, power + 1)) * uniform(rand_engine) + std::pow(min, power + 1),
        1 / (power + 1));
}

array<point_t> generator::generate()
{
    array<point_t> points;

    std::random_device rand_dev;
    std::mt19937 rand_engine(rand_dev());
    std::uniform_real_distribution<real> angle_dist(0.0_r, 2.0_r * M_PI);
    std::uniform_real_distribution<real> position_dist(-params_.max_distance, params_.max_distance);
    std::uniform_real_distribution<real> velocity_dist(params_.min_velocity, params_.max_velocity);

    for (u32 i = 0; i < params_.count; ++i) {
        real angle = angle_dist(rand_engine);

        vec2 position = vec2 { position_dist(rand_engine), position_dist(rand_engine) };
        vec2 velosity = vec2 { std::cos(angle), std::sin(angle) } * velocity_dist(rand_engine);

        point_t point { .position     = position,
                        .velosity     = velosity,
                        .acceleration = vec2 { 0.0_r, 0.0_r },
                        .mass         = power_distribution(params_.min_mass, params_.max_mass, params_.power_mass)
                            * params_.scale_mass };

        points.push_back(point);
    }

    return points;
}

}
