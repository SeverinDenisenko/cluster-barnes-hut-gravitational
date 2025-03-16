#include <cmath>
#include <fstream>
#include <random>
#include <vector>

#include "linalg.hpp"
#include "logging.hpp"
#include "solver.hpp"
#include "types.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    u32 n = 1'000;

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<real> angle_dist(0.0, 2 * M_PI);
    std::uniform_real_distribution<real> distanse_dist(0.0f, 1.0f);
    std::uniform_real_distribution<real> mass_dist(0.0f, 1.0f);

    std::vector<point_t> points;

    for (u32 i = 0; i < n; ++i) {
    retry:

        real distanse = distanse_dist(e2);
        real angle    = angle_dist(e2);

        vec2 position = distanse * vec2 { cos(angle), sin(angle) };
        vec2 velosity = vec2 { -sin(angle), cos(angle) } / n / n;
        point_t point { .position = position, .velosity = velosity, .mass = mass_dist(e2) / n };

        if (distanse < 0.1) {
            goto retry;
        }

        points.push_back(point);
    }

    LOG_INFO("Starting calculation...");

    solver_params nbody_solver_params { .dt = 0.01, .t = 2 * M_PI, .thetha = 0.5 };

    solver nbody_solver { nbody_solver_params, points };

    while (!nbody_solver.finished()) {
        nbody_solver.step();

        LOG_INFO(fmt::format("Done: {:.1f}%", (nbody_solver.time() / nbody_solver_params.t) * 100.0));
    }

    LOG_INFO("Saving results...");

    std::ofstream results("data.txt");
    for (u32 i = 0; i < n; ++i) {
        results << fmt::format("{:+.8f} {:+.8f}\n", points[i].position[0], points[i].position[1]);
    }

    LOG_INFO("Exiting...");

    return 0;
}
