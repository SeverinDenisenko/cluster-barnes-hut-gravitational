#include <fstream>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "generator.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "solver.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    YAML::Node config = YAML::LoadFile("config.yaml");

    real t     = config["solver"]["t"].as<real>();
    real dt    = config["solver"]["dt"].as<real>();
    real theta = config["solver"]["theta"].as<real>();

    u32 count = config["generator"]["count"].as<u32>();

    generator_params params { .count = count };
    generator gen { params };

    std::vector<point_t> points = gen.generate();

    LOG_INFO("Starting calculation...");

    solver_params nbody_solver_params { .t = t, .dt = dt, .theta = theta };

    solver nbody_solver { nbody_solver_params, points };

    while (!nbody_solver.finished()) {
        nbody_solver.rebuild_tree();
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
