#include "frontend.hpp"

#include <raylib.h>
#include <yaml-cpp/yaml.h>

#include "logging.hpp"
#include "model.hpp"

namespace bh {

frontend::frontend(node& node, cluster_transport& transport)
    : node_(node)
    , transport_(transport)
    , screen_size_(800.0_r, 450.0_r)
    , scale_factor_(10.0_r)
{
}

void frontend::start()
{
    YAML::Node config = YAML::LoadFile("config.yaml");

    bool enable_frontend      = config["frontend"]["enable"].as<bool>();
    frontend_refresh_every_   = config["frontend"]["refresh_every"].as<u32>();
    frontend_refresh_counter_ = 0;

    if (!enable_frontend) {
        LOG_INFO("Frontend disabled. Do not starting frontend.");
        return;
    }

    LOG_INFO("Starting frontend application...");

    ev_loop_.start([this]() {
        setup();
        loop();
    });

    LOG_INFO("Exiting frontend application...");
}

void frontend::setup()
{
    solver_params_ = transport_.receive_struct<solver_params>(node_.master_node_index());
    points_        = transport_.receive_array<point_t>(node_.master_node_index());
    solver_        = std::make_unique<solver>(solver_params_, points_, points_);

    InitWindow(screen_size_[0], screen_size_[1], "Simulation");
    SetTargetFPS(0);
}

void frontend::draw()
{
    scale_factor_ += GetMouseWheelMove();

    BeginDrawing();

    ClearBackground(BLACK);

    DrawText(TextFormat("%i fps", GetFPS()), 0, 0, 20, WHITE);

    for (point_t& point : points_) {
        vec2 position = space_to_screen(point.position);
        DrawPixel(position[0], position[1], BLUE);
    }

    EndDrawing();
}

void frontend::get_points()
{
    transport_.receive_array<point_t>(points_.begin(), points_.end(), node_.master_node_index());
}

void frontend::loop()
{
    ev_loop_.push([this]() {
        draw();

        solver_->step(0, 0);

        if (frontend_refresh_counter_ % frontend_refresh_every_ == 0) {
            get_points();
        }
        frontend_refresh_counter_++;

        if (solver_->finished()) {
            stop();
        }

        loop();
    });
}

void frontend::stop()
{
    CloseWindow();
    ev_loop_.stop();
}

vec2 frontend::space_to_screen(vec2 position)
{
    return (position * std::abs(scale_factor_) + screen_size_ / 2);
}

}
