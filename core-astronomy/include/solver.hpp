#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "linalg.hpp"
#include "model.hpp"
#include "tree.hpp"

namespace bh {

struct solver_params {
    real t;
    real dt;
    real theta;
    // softening parameter
    real epsilon;
    real accuracy_parameter;
    bool adaptive_timestep;
};

class solver {
public:
    using quadree = quadtree<point_t, node_t>;

    solver(solver_params params, array<point_t>& points, array<point_t>& points_copy)
        : points_(points)
        , points_copy_(points_copy)
        , params_(params)
        , tree_(quadree::build(points_))
        , t_(0.0_r)
    {
    }

    void rebuild_tree()
    {
        quadree::rebuild(tree_);

        // Compute node masses

        tree_.walk_leafs([](node_t& node, point_t point) { node.mass += point.mass; });

        tree_.walk_nodes([](node_t& parent, node_t& child) { parent.mass += child.mass; });

        // Compute node mass centers

        tree_.walk_leafs(
            [](node_t& node, point_t point) { node.mass_center = point.position * point.mass + node.mass_center; });

        tree_.walk_leafs(
            [](node_t& node, [[maybe_unused]] point_t point) { node.mass_center = node.mass_center / node.mass; });

        tree_.walk_nodes([](node_t& parent, node_t& child) {
            parent.mass_center = child.mass_center * child.mass + parent.mass_center;
        });

        tree_.walk_nodes([](node_t& parent, [[maybe_unused]] node_t& child) {
            parent.mass_center = parent.mass_center / parent.mass;
        });
    }

    void step(u32 begin, u32 end)
    {
        dt_ = calculate_timestap();

        for (u32 i = begin; i < end; ++i) {
            model_body(i);
        }

        std::swap(points_, points_copy_);

        t_ += dt_;
    }

    bool finished()
    {
        return t_ > params_.t;
    }

    real time()
    {
        return t_;
    }

    real total_energy()
    {
        real kinetic   = 0.0_r;
        real potential = 0.0_r;

        for (size_t i = 0; i < points_.size(); ++i) {
            kinetic += 0.5 * points_[i].mass * vec2::dot(points_[i].velocity, points_[i].velocity);
        }

        for (u32 i = 0; i < points_.size(); ++i) {
            for (u32 j = 0; j < points_.size(); ++j) {
                if (i == j) {
                    continue;
                }

                potential
                    += -0.5 * points_[i].mass * points_[j].mass / (points_[i].position - points_[j].position).len();
            }
        }

        return kinetic + potential;
    }

private:
    real calculate_timestap()
    {
        real result = params_.dt;

        if (!params_.adaptive_timestep) {
            return result;
        }

        for (u32 i = 0; i < points_.size(); ++i) {
            for (u32 j = 0; j < points_.size(); ++j) {
                if (i == j) {
                    continue;
                }

                real dt = (points_[i].position - points_[j].position).len()
                    / (points_[i].velocity - points_[j].velocity).len();

                result = std::min(result, dt);
            }
        }

        return result * params_.accuracy_parameter;
    }

    void model_body(u32 i)
    {
        vec2 acceleration { 0.0_r, 0.0_r };

        point_t current = tree_.get_point(i);

        tree_.reduce(
            [&acceleration, &current](const node_t& node) {
                acceleration = acceleration + compute_acceleration(current, node);
            },
            [this, &acceleration, &current](const point_t& point) {
                if (point.position == current.position) {
                    return;
                }

                acceleration = acceleration + compute_acceleration(current, point, params_.epsilon);
            },
            [this, i](quadree::axis_aligned_bounding_box aabb) -> bool {
                return (aabb.max - aabb.min).len() / (points_[i].position - (aabb.max + aabb.min) / 2.0).len()
                    < params_.theta;
            });

        points_copy_[i] = integrator_step(current, acceleration, dt_);
    }

    array<point_t>& points_;
    array<point_t>& points_copy_;
    solver_params params_;
    quadree tree_;
    real t_;
    real dt_;
};

}
