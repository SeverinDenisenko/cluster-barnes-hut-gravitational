#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <vector>

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

class quadtree {
public:
    template <typename T>
    using internal_container = std::vector<T>;
    template <typename T>
    using internal_interator = internal_container<T>::iterator;

    using point_container = internal_container<vec2>;
    using point_iterator  = internal_interator<vec2>;

    static constexpr u32 max_tree_depth = 100;

    static quadtree build(point_container points)
    {
        quadtree tree;

        tree.points_ = std::move(points);
        axis_aligned_bounding_box whole_aabb
            = axis_aligned_bounding_box::create(tree.points_.begin(), tree.points_.end());
        node_id_t root_node_id = build_impl(tree, whole_aabb, tree.points_.begin(), tree.points_.end(), max_tree_depth);
        assert(root_node_id == 0);
        tree.node_points_begin_.push_back(tree.points_.size());

        return tree;
    }

    quadtree(const quadtree&) = delete;
    quadtree(quadtree&&)      = default;

    u32 node_count() const
    {
        return nodes_.size();
    }

private:
    quadtree() = default;

    struct axis_aligned_bounding_box {
        static constexpr real inf = std::numeric_limits<vec2::data_t>::infinity();

        vec2 min { inf, inf };
        vec2 max { -inf, -inf };

        axis_aligned_bounding_box& operator|=(vec2 const& p)
        {
            min = vec2::min(min, p);
            max = vec2::max(max, p);
            return *this;
        }

        template <typename Iterator>
        static axis_aligned_bounding_box create(Iterator begin, Iterator end)
        {
            axis_aligned_bounding_box result;
            for (auto it = begin; it != end; ++it)
                result |= *it;
            return result;
        }
    };

    using node_id_t                               = std::uint32_t;
    static constexpr node_id_t root_node_id       = node_id_t(0);
    static constexpr node_id_t null_child_node_id = node_id_t(root_node_id);

    struct node_t {
        axis_aligned_bounding_box box;
        node_id_t children[2][2] { { null_child_node_id, null_child_node_id },
                                   { null_child_node_id, null_child_node_id } };
    };

    void iterate_node_points(node_id_t id, std::function<void(vec2)> do_something)
    {
        for (auto i = node_points_begin_[id]; i < node_points_begin_[id + 1]; ++i) {
            do_something(points_[i]);
        }
    }

    static node_id_t build_impl(
        quadtree& tree,
        axis_aligned_bounding_box const& bbox,
        point_iterator begin,
        point_iterator end,
        u32 depth_limit)
    {
        if (begin == end) {
            return null_child_node_id;
        }

        node_id_t current_id = tree.nodes_.size();
        tree.nodes_.emplace_back();

        tree.node_points_begin_.emplace_back();
        tree.node_points_begin_[current_id] = std::distance(tree.points_.begin(), begin);

        if (depth_limit == 0) {
            return current_id;
        }

        if (std::equal(begin + 1, end, begin)) {
            return current_id;
        }

        if (begin + 1 == end) {
            return current_id;
        }

        vec2 center = (bbox.min + bbox.max) / 2.0;

        auto bottom = [center](vec2 const& p) { return p[1] < center[1]; };
        auto left   = [center](vec2 const& p) { return p[0] < center[0]; };

        std::vector<vec2>::iterator split_y       = std::partition(begin, end, bottom);
        std::vector<vec2>::iterator split_x_lower = std::partition(begin, split_y, left);
        std::vector<vec2>::iterator split_x_upper = std::partition(split_y, end, left);

        axis_aligned_bounding_box box_0_0      = { bbox.min, center };
        tree.nodes_[current_id].children[0][0] = build_impl(tree, box_0_0, begin, split_x_lower, depth_limit - 1);

        axis_aligned_bounding_box box_0_1      = { vec2 { center[0], bbox.min[1] }, vec2 { bbox.max[0], center[1] } };
        tree.nodes_[current_id].children[0][1] = build_impl(tree, box_0_1, split_x_lower, split_y, depth_limit - 1);

        axis_aligned_bounding_box box_1_0      = { vec2 { bbox.min[0], center[1] }, vec2 { center[0], bbox.max[1] } };
        tree.nodes_[current_id].children[1][0] = build_impl(tree, box_1_0, split_y, split_x_upper, depth_limit - 1);

        axis_aligned_bounding_box box_1_1      = { center, bbox.max };
        tree.nodes_[current_id].children[1][1] = build_impl(tree, box_1_1, split_x_upper, end, depth_limit - 1);

        return current_id;
    }

    point_container points_;
    internal_container<node_t> nodes_;
    internal_container<u32> node_points_begin_;
};

}
