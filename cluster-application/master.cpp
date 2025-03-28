#include "master.hpp"

#include <fstream>

#include <yaml-cpp/yaml.h>

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

master_node::master_node(node& node, cluster_transport& transport)
    : node_(node)
    , transport_(transport)
{
}

void master_node::start()
{
    LOG_INFO("Starting master application...");

    ev_loop_.start([this]() {
        setup();
        loop();
    });

    LOG_INFO("Exiting master application...");
}

void master_node::setup()
{
    YAML::Node config = YAML::LoadFile("config.yaml");

    enable_frontend_          = config["frontend"]["enable"].as<bool>();
    frontend_refresh_every_   = config["frontend"]["refresh_every"].as<u32>();
    frontend_refresh_counter_ = 0;

    enable_output_ = config["output"]["enable"].as<bool>();

    generator_params generator_params { .count          = config["generator"]["count"].as<u32>(),
                                        .min_mass       = config["generator"]["min_mass"].as<real>(),
                                        .max_mass       = config["generator"]["max_mass"].as<real>(),
                                        .power_mass     = config["generator"]["power_mass"].as<real>(),
                                        .scale_mass     = config["generator"]["scale_mass"].as<real>(),
                                        .max_distance   = config["generator"]["max_distance"].as<real>(),
                                        .scale_velocity = config["generator"]["scale_velocity"].as<real>() };

    solver_params_ = solver_params { .t       = config["solver"]["t"].as<real>(),
                                     .dt      = config["solver"]["dt"].as<real>(),
                                     .theta   = config["solver"]["theta"].as<real>(),
                                     .epsilon = config["solver"]["epsilon"].as<real>() };

    points_      = generator { generator_params }.generate();
    points_copy_ = points_;

    send_parameters();
    send_points();
    send_to_frontend();

    nbody_solver_ = std::make_unique<solver>(solver_params_, points_, points_copy_);

    send_chunks();
}

void master_node::send_points()
{
    for (u32 node : node_.slaves_node_indexes()) {
        transport_.send_array<point_t>(points_.begin(), points_.end(), node);

        LOG_TRACE(fmt::format("Send points: node={}, size={}", node, points_.size()));
    }
}

void master_node::send_chunks()
{
    slaves_         = node_.slaves_node_indexes();
    working_chunks_ = make_chunks(points_.size(), slaves_.size());

    for (u32 i = 0; i < slaves_.size(); ++i) {
        transport_.send_struct<chunk>(working_chunks_[i], slaves_[i]);

        LOG_INFO(fmt::format(
            "Send chunk: node={}, begin={}, end={}", slaves_[i], working_chunks_[i].begin, working_chunks_[i].end));
    }
}

void master_node::stop()
{
    ev_loop_.stop();
}

void master_node::send_parameters()
{
    for (u32 node : node_.slaves_node_indexes()) {
        transport_.send_struct<solver_params>(solver_params_, node);

        LOG_TRACE(fmt::format("Send params: node={}", node));
    }

    if (enable_frontend_) {
        transport_.send_struct<solver_params>(solver_params_, node_.frontend_node_index());
    }
}

void master_node::get_solutions()
{
    for (u32 i = 0; i < slaves_.size(); ++i) {
        transport_.receive_array<point_t>(
            points_.begin() + working_chunks_[i].begin, points_.begin() + working_chunks_[i].end, slaves_[i]);

        LOG_TRACE(fmt::format(
            "Got solutin: node={}, begin={}, end={}", slaves_[i], working_chunks_[i].begin, working_chunks_[i].end));
    }
}

void master_node::rebuild_tree()
{
    nbody_solver_->rebuild_tree();
}

void master_node::write_results()
{
    std::ofstream out("data.txt");

    for (point_t& point : points_) {
        out << fmt::format("{:.8f} {:.8f}\n", point.position[0], point.position[1]);
    }
}

void master_node::send_to_frontend()
{
    transport_.send_array<point_t>(points_.begin(), points_.end(), node_.frontend_node_index());
}

void master_node::loop()
{
    ev_loop_.push([this]() {
        get_solutions();
        rebuild_tree();
        send_points();

        nbody_solver_->step(0, 0);

        if (enable_frontend_ && frontend_refresh_counter_ % frontend_refresh_every_ == 0) {
            send_to_frontend();
        }
        frontend_refresh_counter_++;

        if (nbody_solver_->finished()) {
            if (enable_output_) {
                write_results();
            }
            stop();
        }

        loop();
    });
}

}
