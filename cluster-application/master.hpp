#pragma once

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "frontend.hpp"
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

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    bool enable_frontend_;
    bool enable_output_;
    frontend frontend_;
    solver_params solver_params_;
    array<u32> slaves_;
    array<chunk> working_chunks_;
    array<point_t> points_;
    std::unique_ptr<solver> nbody_solver_;
};

}
