#include "frontend.hpp"

#include <limits>

#include "raylib.h"

#include "linalg.hpp"
#include "model.hpp"
#include "types.hpp"

namespace bh {

frontend::frontend(array<point_t>& points)
    : points_(points)
    , screen_size_(800.0_r, 450.0_r)
    , scale_factor_(10.0_r)
{
}

void frontend::open()
{
    InitWindow(screen_size_[0], screen_size_[1], "Simulation");
    SetTargetFPS(0);
}

void frontend::update()
{
    scale_factor_ += GetMouseWheelMove();

    BeginDrawing();

    ClearBackground(BLACK);

    DrawText(TextFormat("%i fps", GetFPS()), 0, 0, 20, WHITE);

    real max_mass = -std::numeric_limits<real>::infinity();
    real min_mass = std::numeric_limits<real>::infinity();

    for (point_t& point : points_) {
        max_mass = std::max(max_mass, point.mass);
        min_mass = std::min(min_mass, point.mass);
    }

    for (point_t& point : points_) {
        vec2 position = space_to_screen(point.position);
        DrawCircle(position[0], position[1], mass_to_radius(point.mass, min_mass, max_mass), BLUE);
    }

    EndDrawing();
}

void frontend::close()
{
    CloseWindow();
}

vec2 frontend::space_to_screen(vec2 position)
{
    return (position * std::abs(scale_factor_) + screen_size_ / 2);
}

real frontend::mass_to_radius(real mass, real min_mass, real max_mass)
{
    return 1.0 + 1.0 * (max_mass - mass) / (max_mass - min_mass);
}

}
