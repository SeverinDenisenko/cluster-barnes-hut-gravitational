#pragma once

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

struct point_t {
    vec2 position {};
    vec2 velocity {};
    real mass {};
};

struct node_t {
    real mass {};
    vec2 mass_center {};
};

inline vec2 compute_acceleration(const point_t& a, const point_t& b, real epsilon)
{
    vec2 r   = a.position - b.position;
    real len = r.len() + epsilon;
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

inline point_t integrator_step(point_t p, vec2 acceleration, real dt)
{
    p.velocity = p.velocity + dt * acceleration;
    p.position = p.position + dt * p.velocity;
    return p;
}

}
