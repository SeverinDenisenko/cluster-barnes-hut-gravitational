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
    std::uniform_real_distribution<real> position_dist(0, params_.max_distance);

    for (u32 i = 0; i < params_.count; ++i) {
        real angle = angle_dist(rand_engine);

        real r        = position_dist(rand_engine);
        vec2 position = vec2 { std::cos(angle), std::sin(angle) } * r;
        real mass     = power_distribution(params_.min_mass, params_.max_mass, params_.power_mass) * params_.scale_mass;

        real v = r * params_.scale_velocity;

        vec2 velosity = vec2 { -std::sin(angle), std::cos(angle) } * v;

        point_t point { .position = position, .velosity = velosity, .mass = mass };

        points.push_back(point);
    }

    return points;
}

}
