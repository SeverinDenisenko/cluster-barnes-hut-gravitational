#include "frontend.hpp"

#include <raylib.h>
#include <utility>
#include <yaml-cpp/yaml.h>

#include "ev_loop.hpp"
#include "logging.hpp"
#include "messages.hpp"
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

    bool enable_frontend    = config["frontend"]["enable"].as<bool>();
    frontend_refresh_every_ = config["frontend"]["refresh_every"].as<u32>();

    if (!enable_frontend) {
        LOG_INFO("Frontend disabled. Do not starting frontend.");
        return;
    }

    LOG_INFO("Starting frontend application...");

    startEvLoop([this](unit) -> unit {
        setup();

        return unit();
    });

    LOG_INFO("Exiting frontend application...");
}

void frontend::setup()
{
    transport_.add_handler<solver_params>(
        node_.master_node_index(),
        std::to_underlying(cluster_messages::solver_params),
        [this](solver_params solver_params) -> unit {
            solver_params_ = solver_params;

            points_ = transport_.receive_array<point_t>(
                node_.master_node_index(), std::to_underlying(cluster_messages::points));
            solver_ = std::make_unique<solver>(solver_params_, points_, points_);

            InitWindow(screen_size_[0], screen_size_[1], "Simulation");
            SetTargetFPS(0);

            transport_.add_handler<point_t>(
                node_.master_node_index(),
                std::to_underlying(cluster_messages::points),
                [this](array<point_t> points) -> unit {
                    points_ = points;

                    for (u32 i = 0; i < frontend_refresh_every_; ++i) {
                        solver_->step(0, 0);
                    }

                    return unit();
                },
                true);

            transport_.receive_array<point_t>(
                points_.begin(),
                points_.end(),
                node_.master_node_index(),
                std::to_underlying(cluster_messages::points));

            loop();

            return unit();
        });
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

void frontend::loop()
{
    pushToEvLoop<unit>([this](unit) -> unit {
        draw();

        solver_->step(0, 0);

        if (solver_->finished()) {
            stop();
        }

        loop();

        return unit();
    });
}

void frontend::stop()
{
    CloseWindow();
    stopEvLoop();
}

vec2 frontend::space_to_screen(vec2 position)
{
    return (position * std::abs(scale_factor_) + screen_size_ / 2);
}

}
