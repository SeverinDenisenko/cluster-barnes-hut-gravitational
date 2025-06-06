#include "frontend.hpp"

#include <algorithm>
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
    , screen_size_(900.0_r, 500.0_r)
    , scale_factor_(10.0_r)
{
}

void frontend::start()
{
    YAML::Node config = YAML::LoadFile("config.yaml");

    bool enable_frontend    = config["frontend"]["enable"].as<bool>();
    frontend_refresh_every_ = config["frontend"]["refresh_every"].as<u32>();
    draw_energy_            = config["frontend"]["draw_energy"].as<bool>();

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
    points_ = transport_.receive_array<point_t>(
        node_.master_node_index(), std::to_underlying(cluster_message_type::points));

    InitWindow(screen_size_[0], screen_size_[1], "Simulation");
    SetTargetFPS(0);

    transport_.add_handler<points_message>(
        node_.master_node_index(), [](points_message) -> unit { return unit(); }, points_message { points_ }, true);

    transport_.add_handler<stop_message>(
        node_.master_node_index(),
        [this](stop_message) -> unit {
            stop();
            return unit();
        },
        stop_message {},
        true);

    transport_.add_handler<status_message>(
        node_.master_node_index(),
        [this](status_message msg) -> unit {
            done_persent_ = msg.status_.done_persent;
            energy_vec_.push_back(msg.status_.energy);
            return unit();
        },
        status_message { status { .done_persent = 0.0_r, .energy = 0.0_r } },
        true);

    loop();
}

void frontend::draw()
{
    scale_factor_ += GetMouseWheelMove();
    if (scale_factor_ < 0) {
        scale_factor_ = 0;
    }

    BeginDrawing();

    ClearBackground(BLACK);

    DrawText(TextFormat("%i frames per second", GetFPS()), 0, 0, 20, WHITE);
    DrawText(TextFormat("%02.02f percent done", done_persent_), 0, 25, 20, WHITE);

    if (draw_energy_ && !energy_vec_.empty()) {
        real max = *std::max_element(energy_vec_.begin(), energy_vec_.end());
        real min = *std::min_element(energy_vec_.begin(), energy_vec_.end());

        DrawText(TextFormat("%02.02f", max), screen_size_[0] - 20 * 4, 0, 20, RED);
        DrawText(TextFormat("%02.02f", min), screen_size_[0] - 20 * 4, screen_size_[1] - 20, 20, RED);

        for (size_t i = 0; i < energy_vec_.size() - 1; ++i) {
            DrawLine(
                (float)i / energy_vec_.size() * screen_size_[0],
                screen_size_[1] - (energy_vec_[i] - min) / (max - min) * screen_size_[1],
                (float)(i + 1) / energy_vec_.size() * screen_size_[0],
                screen_size_[1] - (energy_vec_[i + 1] - min) / (max - min) * screen_size_[1],
                RED);
        }
    }

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
