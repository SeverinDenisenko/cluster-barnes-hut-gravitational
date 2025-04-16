#include "slave.hpp"

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "logging.hpp"
#include "messages.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

slave_node::slave_node(node& node, cluster_transport& transport)
    : node_(node)
    , transport_(transport)
{
}

void slave_node::start()
{
    LOG_INFO("Starting slave application...");

    startEvLoop([this](unit) -> unit {
        setup();

        return unit();
    });

    LOG_INFO("Exiting slave application...");
}

void slave_node::setup()
{
    get_parameters();
}

void slave_node::get_points()
{
    points_ = transport_.receive_array<point_t>(
        node_.master_node_index(), std::to_underlying(cluster_message_type::points));
    points_copy_ = points_;

    LOG_TRACE(fmt::format("[node: {}] Got points: size={}", node_.node_index(), points_.size()));

    nbody_solver_ = std::make_unique<solver>(solver_params_, points_, points_copy_);

    transport_.add_handler<chunk_message>(
        node_.master_node_index(),
        [this](chunk_message msg) -> unit {
            working_chunk_ = msg.chunk_;

            LOG_INFO(fmt::format(
                "[node: {}] Got chunk: begin={}, end={}",
                node_.node_index(),
                working_chunk_.begin,
                working_chunk_.end));

            loop();

            return unit();
        },
        chunk_message { working_chunk_ });
}

void slave_node::update_points()
{
    transport_.receive_array<point_t>(
        points_.begin(), points_.end(), node_.master_node_index(), std::to_underlying(cluster_message_type::points));

    LOG_TRACE(fmt::format("[node: {}] Updated points: size={}", node_.node_index(), points_.size()));
}

void slave_node::solve()
{
    nbody_solver_->step(working_chunk_.begin, working_chunk_.end);
}

void slave_node::send_solution()
{
    transport_.send_array<point_t>(
        points_.begin() + working_chunk_.begin,
        points_.begin() + working_chunk_.end,
        node_.master_node_index(),
        std::to_underlying(cluster_message_type::points));

    LOG_TRACE(fmt::format(
        "[node: {}] Send solutin: begin={}, end={}", node_.node_index(), working_chunk_.begin, working_chunk_.end));
}

void slave_node::get_parameters()
{
    transport_.add_handler<solver_params_message>(
        node_.master_node_index(),
        [this](solver_params_message msg) -> unit {
            solver_params_ = msg.params_;

            LOG_TRACE(fmt::format("[node: {}] Got params", node_.node_index()));

            get_points();

            return unit();
        },
        solver_params_message { solver_params_ });
}

void slave_node::stop()
{
    stopEvLoop();
}

void slave_node::rebuild_tree()
{
    nbody_solver_->rebuild_tree();
}

void slave_node::loop()
{
    pushToEvLoop<unit>([this](unit) -> unit {
        solve();
        send_solution();
        update_points();
        rebuild_tree();

        if (nbody_solver_->finished()) {
            stop();
        }

        loop();

        return unit();
    });
}

}
