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

class master_node {
public:
    master_node(node& node, cluster_transport& transport);

    void start();

private:
    void setup();

    void send_points();

    void send_chunks();

    void stop();

    void send_parameters();

    void get_solutions();

    void rebuild_tree();

    void loop();

    void write_results();

    void send_to_frontend();

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    bool enable_output_;
    bool enable_frontend_;
    solver_params solver_params_;
    array<u32> slaves_;
    array<chunk> working_chunks_;
    array<point_t> points_;
    array<point_t> points_copy_;
    std::unique_ptr<solver> nbody_solver_;
    u32 frontend_refresh_every_;
    u32 frontend_refresh_counter_;
};

}
