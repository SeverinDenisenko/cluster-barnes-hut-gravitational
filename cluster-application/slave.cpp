#include "slave.hpp"

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

slave_node::slave_node(node& node, cluster_transport& transport)
    : node_(node)
    , transport_(transport)
    , ev_loop_()
{
}

void slave_node::start()
{
    LOG_INFO("Starting slave application...");

    ev_loop_.start([this]() {
        setup();
        loop();
    });

    LOG_INFO("Exiting slave application...");
}

void slave_node::setup()
{
    get_parameters();
    get_points();

    nbody_solver_ = std::make_unique<solver>(solver_params_, points_);

    get_chunk();
}

void slave_node::get_points()
{
    points_ = transport_.receive_array<point_t>(node_.master_node_index());

    LOG_TRACE(fmt::format("[node: {}] Got points: size={}", node_.node_index(), points_.size()));

    node_.sync_cluster();
}

void slave_node::update_points()
{
    transport_.receive_array<point_t>(points_.begin(), points_.end(), node_.master_node_index());

    LOG_TRACE(fmt::format("[node: {}] Updated points: size={}", node_.node_index(), points_.size()));

    node_.sync_cluster();
}

void slave_node::get_chunk()
{
    working_chunk_ = transport_.receive_struct<chunk>(node_.master_node_index());

    LOG_INFO(fmt::format(
        "[node: {}] Got chunk: begin={}, end={}", node_.node_index(), working_chunk_.begin, working_chunk_.end));

    node_.sync_cluster();
}

void slave_node::solve()
{
    nbody_solver_->step(working_chunk_.begin, working_chunk_.end);
}

void slave_node::send_solution()
{
    transport_.send_array<point_t>(
        points_.begin() + working_chunk_.begin, points_.begin() + working_chunk_.end, node_.master_node_index());

    LOG_TRACE(fmt::format(
        "[node: {}] Send solutin: begin={}, end={}", node_.node_index(), working_chunk_.begin, working_chunk_.end));

    node_.sync_cluster();
}

void slave_node::get_parameters()
{
    solver_params_ = transport_.receive_struct<solver_params>(node_.master_node_index());

    LOG_TRACE(fmt::format("[node: {}] Got params", node_.node_index()));

    node_.sync_cluster();
}

void slave_node::stop()
{
    ev_loop_.stop();
}

void slave_node::rebuild_tree()
{
    nbody_solver_->rebuild_tree();
}

void slave_node::loop()
{
    ev_loop_.push([this]() {
        solve();
        send_solution();
        update_points();
        rebuild_tree();

        loop();

        if (nbody_solver_->finished()) {
            stop();
        }
    });
}

}
