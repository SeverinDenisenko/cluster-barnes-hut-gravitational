#pragma once

#include <algorithm>
#include <vector>

#include "linalg.hpp"
#include "model.hpp"
#include "tree.hpp"

namespace bh {

struct solver_params {
    real t;
    real dt;
    real thetha;
};

class solver {
public:
    using quadree = quadtree<point_t, node_t>;

    solver(solver_params params, array<point_t>& points)
        : points_(points)
        , points_copy_(points_)
        , params_(params)
        , tree_(quadree::build(points_))
        , t_(0.0f)
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

    void step()
    {
        for (u32 i = 0; i < points_.size(); ++i) {
            model_body(i);
        }

        std::swap(points_, points_copy_);

        t_ += params_.dt;
    }

    void step(u32 begin, u32 end)
    {
        for (u32 i = begin; i < end; ++i) {
            model_body(i);
        }

        std::swap(points_, points_copy_);

        t_ += params_.dt;
    }

    bool finished()
    {
        return t_ > params_.t;
    }

    real time()
    {
        return t_;
    }

private:
    void model_body(u32 i)
    {
        vec2 acceleration { 0.0f, 0.0f };

        const point_t& current = tree_.get_point(i);

        tree_.reduce(
            [&acceleration, &current](const node_t& node) {
                acceleration = acceleration + compute_acceleration(current, node);
            },
            [&acceleration, &current](const point_t& point) {
                if (point.position == current.position) {
                    return;
                }

                acceleration = acceleration + compute_acceleration(current, point);
            },
            [this, i](quadree::axis_aligned_bounding_box aabb) -> bool {
                return (aabb.max - aabb.min).len() / (points_[i].position - (aabb.max + aabb.min) / 2.0).len()
                    < params_.thetha;
            });

        points_copy_[i].position = compute_position(current, acceleration, params_.dt);
        points_copy_[i].velosity = compute_velosity(current, acceleration, params_.dt);
    }

    array<point_t>& points_;
    array<point_t> points_copy_;
    solver_params params_;
    quadree tree_;
    real t_;
};

}
