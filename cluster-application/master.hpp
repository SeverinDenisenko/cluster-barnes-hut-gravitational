#pragma once

#include <iterator>

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "generator.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "solver.hpp"
#include "transport.hpp"
#include "types.hpp"

namespace bh {

class master_node {
public:
    master_node(node& node, cluster_transport& transport)
        : node_(node)
        , transport_(transport)
    {
    }

    void start()
    {
        LOG_INFO("Starting master application...");

        ev_loop_.start([this]() { loop(); });

        ev_loop_.join();

        LOG_INFO("Exiting master application...");
    }

private:
    void send_points()
    {
        for (u32 node : node_.slaves_node_indexes()) {
            transport_.send_array<point_t>(points_.begin(), points_.end(), node);

            LOG_INFO(fmt::format("Send points: node={}, size={}", node, points_.size()));
        }
    }

    void send_chunks()
    {
        slaves_         = node_.slaves_node_indexes();
        working_chunks_ = make_chunks(points_.size(), slaves_.size());

        for (u32 i = 0; i < slaves_.size(); ++i) {
            transport_.send_struct<chunk>(working_chunks_[i], slaves_[i]);

            LOG_INFO(fmt::format(
                "Send chunk: node={}, begin={}, end={}", slaves_[i], working_chunks_[i].begin, working_chunks_[i].end));
        }
    }

    void stop()
    {
        ev_loop_.stop();
    }

    void send_parameters()
    {
        for (u32 node : node_.slaves_node_indexes()) {
            transport_.send_struct<solver_params>(solver_params_, node);

            LOG_INFO(fmt::format("Send params: node={}", node));
        }
    }

    void get_solutions()
    {
        for (u32 i = 0; i < slaves_.size(); ++i) {
            transport_.receive_array<point_t>(
                points_.begin() + working_chunks_[i].begin, points_.begin() + working_chunks_[i].end, slaves_[i]);

            LOG_INFO(fmt::format(
                "Got solutin: node={}, begin={}, end={}",
                slaves_[i],
                working_chunks_[i].begin,
                working_chunks_[i].end));
        }
    }

    void rebuild_tree()
    {
        // todo
    }

    void loop()
    {
        points_ = generator { generator_params { .count = 1'000 } }.generate();

        send_parameters();
        send_points();
        send_chunks();

        ev_loop_.push([this]() {
            get_solutions();
            rebuild_tree();
            send_points();
            loop();

            // todo remove
            stop();
        });
    }

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    solver_params solver_params_;
    array<u32> slaves_;
    array<chunk> working_chunks_;
    array<point_t> points_;
};

}
