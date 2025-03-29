#pragma once

#include <memory>

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

class slave_node {
public:
    slave_node(node& node, cluster_transport& transport);

    void start();

private:
    void setup();

    void get_points();

    void update_points();

    void get_chunk();

    void solve();

    void send_solution();

    void get_parameters();

    void stop();

    void rebuild_tree();

    void loop();

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    solver_params solver_params_;
    array<point_t> points_;
    array<point_t> points_copy_;
    chunk working_chunk_;
    std::unique_ptr<solver> nbody_solver_;
};

}
