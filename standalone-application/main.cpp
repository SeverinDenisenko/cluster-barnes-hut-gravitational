#include <cmath>
#include <fstream>
#include <random>
#include <vector>

#include "generator.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "solver.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    generator_params params { .count = 1'000 };
    generator gen { params };

    std::vector<point_t> points = gen.generate();

    LOG_INFO("Starting calculation...");

    solver_params nbody_solver_params { .dt = 0.01, .t = 2 * M_PI, .thetha = 0.5 };

    solver nbody_solver { nbody_solver_params, points };

    while (!nbody_solver.finished()) {
        nbody_solver.step();

        LOG_INFO(fmt::format("Done: {:.1f}%", (nbody_solver.time() / nbody_solver_params.t) * 100.0));
    }

    LOG_INFO("Saving results...");

    std::ofstream results("data.txt");
    for (u32 i = 0; i < params.count; ++i) {
        results << fmt::format("{:+.8f} {:+.8f}\n", points[i].position[0], points[i].position[1]);
    }

    LOG_INFO("Exiting...");

    return 0;
}
