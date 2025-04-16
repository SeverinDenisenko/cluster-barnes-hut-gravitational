#pragma once

#include <memory>

#include "cluster.hpp"
#include "linalg.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"

namespace bh {

class frontend {
public:
    frontend(node& node, cluster_transport& transport);

    void start();

private:
    void setup();

    void stop();

    void loop();

    void draw();

    vec2 space_to_screen(vec2 position);

    node& node_;
    cluster_transport& transport_;
    array<point_t> points_;
    solver_params solver_params_;
    std::unique_ptr<solver> solver_;
    vec2 screen_size_;
    real scale_factor_;
    u32 frontend_refresh_every_;
};

}
