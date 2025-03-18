#pragma once

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

class slave_node {
public:
    slave_node(node& node, cluster_transport& transport)
        : node_(node)
        , transport_(transport)
        , ev_loop_()
    {
    }

    void start()
    {
        LOG_INFO("Starting slave application...");

        ev_loop_.start([this]() { loop(); });

        ev_loop_.join();

        LOG_INFO("Exiting slave application...");
    }

private:
    void get_points()
    {
        points_ = transport_.receive_array<point_t>(node_.master_node_index());

        LOG_INFO(fmt::format("[node: {}] Got points: size={}", node_.node_index(), points_.size()));
    }

    void update_points()
    {
        transport_.receive_array<point_t>(points_.begin(), points_.end(), node_.master_node_index());

        LOG_INFO(fmt::format("[node: {}] Updated points: size={}", node_.node_index(), points_.size()));
    }

    void get_chunk()
    {
        working_chunk_ = transport_.receive_struct<chunk>(node_.master_node_index());

        LOG_INFO(fmt::format(
            "[node: {}] Got chunk: begin={}, end={}", node_.node_index(), working_chunk_.begin, working_chunk_.end));
    }

    void solve()
    {
        // todo
    }

    void send_solution()
    {
        transport_.send_array<point_t>(
            points_.begin() + working_chunk_.begin, points_.begin() + working_chunk_.end, node_.master_node_index());

        LOG_INFO(fmt::format(
            "[node: {}] Send solutin: begin={}, end={}", node_.node_index(), working_chunk_.begin, working_chunk_.end));
    }

    void get_parameters()
    {
        solver_params_ = transport_.receive_struct<solver_params>(node_.master_node_index());

        LOG_INFO(fmt::format("[node: {}] Got params", node_.node_index()));
    }

    void stop()
    {
        ev_loop_.stop();
    }

    void rebuild_tree()
    {
        // todo
    }

    void loop()
    {
        get_parameters();
        get_points();
        rebuild_tree();
        get_chunk();

        ev_loop_.push([this]() {
            solve();
            send_solution();
            update_points();
            rebuild_tree();

            loop();

            // todo remove
            stop();
        });
    }

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    solver_params solver_params_;
    array<point_t> points_;
    chunk working_chunk_;
};

}
