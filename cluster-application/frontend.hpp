#pragma once

#include "raylib.h"

#include "linalg.hpp"
#include "model.hpp"
#include "types.hpp"

namespace bh {
class frontend {
public:
    frontend(array<point_t>& points)
        : points_(points)
        , screen_size_(800.0_r, 450.0_r)
    {
    }

    void open()
    {
        InitWindow(screen_size_[0], screen_size_[1], "Simulation");
        SetTargetFPS(60);
    }

    void update()
    {
        BeginDrawing();

        ClearBackground(BLACK);

        for (point_t& point : points_) {
            vec2 position = space_to_screen(point.position);
            DrawCircle(position[0], position[1], point.mass, BLUE);
        }

        EndDrawing();
    }

    void close()
    {
        CloseWindow();
    }

private:
    vec2 space_to_screen(vec2 position)
    {
        return (position * 10.0 + screen_size_ / 2);
    }

    array<point_t>& points_;
    vec2 screen_size_;
};

}
