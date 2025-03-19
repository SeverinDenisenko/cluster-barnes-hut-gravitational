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
    std::uniform_real_distribution<real> angle_dist(0.0, 2 * M_PI);
    std::uniform_real_distribution<real> distanse_dist(0.0f, 1.0f);
    std::uniform_real_distribution<real> mass_dist(0.0f, 1.0f);

    for (u32 i = 0; i < params_.count; ++i) {

        real distanse = distanse_dist(rand_engine);
        real angle    = angle_dist(rand_engine);

        vec2 position = distanse * vec2 { std::cos(angle), std::sin(angle) };
        vec2 velosity = vec2 { -std::sin(angle), std::cos(angle) } / params_.count;
        point_t point { .position = position, .velosity = velosity, .mass = mass_dist(rand_engine) / params_.count };

        points.push_back(point);
    }

    return points;
}

}
