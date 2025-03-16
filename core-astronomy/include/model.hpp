#pragma once

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

struct point_t {
    vec2 position {};
    vec2 velosity {};
    real mass {};
};

struct node_t {
    real mass {};
    vec2 mass_center {};
};

inline vec2 compute_acceleration(const point_t& a, const point_t& b)
{
    vec2 r   = a.position - b.position;
    real len = std::max(r.len(), 0.001f);
    real r3  = len * len * len;
    return -r * b.mass / r3;
}

inline vec2 compute_acceleration(const point_t& a, const node_t& n)
{
    vec2 r   = a.position - n.mass_center;
    real len = r.len();
    real r3  = len * len * len;
    return -r * n.mass / r3;
}

inline vec2 compute_position(const point_t& a, vec2 acceleration, real dt)
{
    return a.position + a.velosity * dt + acceleration * dt * dt / 2.0;
}

inline vec2 compute_velosity(const point_t& a, vec2 acceleration, real dt)
{
    return a.velosity + acceleration * dt;
}

}
