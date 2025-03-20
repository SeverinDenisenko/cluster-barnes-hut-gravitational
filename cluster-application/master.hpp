#pragma once

#include <yaml-cpp/yaml.h>

#include "chunks.hpp"
#include "cluster.hpp"
#include "ev_loop.hpp"
#include "frontend.hpp"
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
        , frontend_(points_)
    {
    }

    void start()
    {
        LOG_INFO("Starting master application...");

        ev_loop_.start([this]() {
            setup();
            loop();
        });

        LOG_INFO("Exiting master application...");
    }

private:
    void setup()
    {
        frontend_.open();

        YAML::Node config = YAML::LoadFile("config.yaml");

        generator_params generator_params { .count        = config["generator"]["count"].as<u32>(),
                                            .min_mass     = config["generator"]["min_mass"].as<real>(),
                                            .max_mass     = config["generator"]["max_mass"].as<real>(),
                                            .power_mass   = config["generator"]["power_mass"].as<real>(),
                                            .scale_mass   = config["generator"]["scale_mass"].as<real>(),
                                            .max_distance = config["generator"]["max_distance"].as<real>(),
                                            .min_velocity = config["generator"]["min_velocity"].as<real>(),
                                            .max_velocity = config["generator"]["max_velocity"].as<real>() };

        points_ = generator { generator_params }.generate();

        solver_params_ = solver_params { .t     = config["solver"]["t"].as<real>(),
                                         .dt    = config["solver"]["dt"].as<real>(),
                                         .theta = config["solver"]["theta"].as<real>() };

        send_parameters();
        send_points();

        nbody_solver_ = std::make_unique<solver>(solver_params_, points_);

        send_chunks();
    }

    void send_points()
    {
        for (u32 node : node_.slaves_node_indexes()) {
            transport_.send_array<point_t>(points_.begin(), points_.end(), node);

            LOG_TRACE(fmt::format("Send points: node={}, size={}", node, points_.size()));
        }

        node_.sync_cluster();
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

        node_.sync_cluster();
    }

    void stop()
    {
        ev_loop_.stop();
    }

    void send_parameters()
    {
        for (u32 node : node_.slaves_node_indexes()) {
            transport_.send_struct<solver_params>(solver_params_, node);

            LOG_TRACE(fmt::format("Send params: node={}", node));
        }

        node_.sync_cluster();
    }

    void get_solutions()
    {
        for (u32 i = 0; i < slaves_.size(); ++i) {
            transport_.receive_array<point_t>(
                points_.begin() + working_chunks_[i].begin, points_.begin() + working_chunks_[i].end, slaves_[i]);

            LOG_TRACE(fmt::format(
                "Got solutin: node={}, begin={}, end={}",
                slaves_[i],
                working_chunks_[i].begin,
                working_chunks_[i].end));
        }

        node_.sync_cluster();
    }

    void rebuild_tree()
    {
        nbody_solver_->rebuild_tree();
    }

    void loop()
    {
        ev_loop_.push([this]() {
            get_solutions();
            frontend_.update();
            rebuild_tree();
            send_points();
            loop();

            nbody_solver_->step(0, 0);

            LOG_INFO(fmt::format("Done: {:.1f}%", (nbody_solver_->time() / solver_params_.t) * 100.0));

            if (nbody_solver_->finished()) {
                frontend_.close();
                stop();
            }
        });
    }

    node& node_;
    cluster_transport& transport_;
    ev_loop ev_loop_;
    frontend frontend_;
    solver_params solver_params_;
    array<u32> slaves_;
    array<chunk> working_chunks_;
    array<point_t> points_;
    std::unique_ptr<solver> nbody_solver_;
};

}
