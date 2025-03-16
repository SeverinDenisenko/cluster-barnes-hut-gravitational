#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <vector>

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

template <typename PositionalData, typename NodeData>
class quadtree {
public:
    template <typename T>
    using internal_container = std::vector<T>;
    template <typename T>
    using internal_interator = internal_container<T>::iterator;

    static constexpr u32 max_tree_depth = 100;
    static constexpr u32 tree_dimention = 2;

    using point = vec<tree_dimention>;

    using point_container = internal_container<PositionalData>;
    using point_iterator  = internal_interator<PositionalData>;

    struct axis_aligned_bounding_box {
        static constexpr real inf = std::numeric_limits<point::data_t>::infinity();

        point min { inf, inf };
        point max { -inf, -inf };

        axis_aligned_bounding_box& operator|=(point const& p)
        {
            min = point::min(min, p);
            max = point::max(max, p);
            return *this;
        }

        static axis_aligned_bounding_box create(point_iterator begin, point_iterator end)
        {
            axis_aligned_bounding_box result;
            for (auto it = begin; it != end; ++it)
                result |= it->position;
            return result;
        }
    };

    static quadtree build(point_container& points)
    {
        quadtree tree(points);

        tree.build_tree();

        return tree;
    }

    static void rebuild(quadtree& tree)
    {
        tree.destroy_tree();
        tree.build_tree();
    }

    quadtree(const quadtree&) = delete;
    quadtree(quadtree&&)      = default;

    u32 node_count() const
    {
        return nodes_.size();
    }

    u32 depth() const
    {
        return depth_;
    }

    const NodeData& get_node(u32 i) const
    {
        return nodes_[i].data;
    }

    const PositionalData& get_point(u32 i) const
    {
        return points_[i];
    }

    void walk_leafs(std::function<void(NodeData&, PositionalData&)> reduce_leafs)
    {
        for (node_id_t id = 0; id < nodes_.size(); ++id) {
            node_id_t node = nodes_.size() - 1 - id;

            if (!nodes_[node].is_leaf()) {
                continue;
            }

            for (u32 point = node_points_begin_[node]; point < node_points_begin_[node + 1]; ++point) {
                reduce_leafs(nodes_[node].data, points_[point]);
            }
        }
    }

    void walk_nodes(std::function<void(NodeData&, NodeData&)> reduce_node)
    {
        for (node_id_t id = 0; id < nodes_.size(); ++id) {
            node_id_t node = nodes_.size() - 1 - id;

            if (nodes_[node].is_leaf()) {
                continue;
            }

            for (u32 child = 0; child < node_child_count; ++child) {
                if (nodes_[node].children[child] == null_child_node_id) {
                    continue;
                }
                reduce_node(nodes_[node].data, nodes_[nodes_[node].children[child]].data);
            }
        }
    }

    void reduce(
        std::function<void(const NodeData&)> reduce_node,
        std::function<void(const PositionalData&)> reduce_point,
        std::function<bool(const axis_aligned_bounding_box aabb)> stop_condition) const
    {
        reduce_impl(root_node_id, reduce_node, reduce_point, stop_condition);
    }

private:
    using node_id_t = std::uint32_t;

    void reduce_impl(
        node_id_t current,
        std::function<void(const NodeData&)>& reduce_node,
        std::function<void(const PositionalData&)>& reduce_point,
        std::function<bool(const axis_aligned_bounding_box aabb)>& stop_condition) const
    {
        if (stop_condition(nodes_[current].box)) {
            reduce_node(nodes_[current].data);
        } else {
            if (nodes_[current].is_leaf()) {
                for (u32 point = node_points_begin_[current]; point < node_points_begin_[current + 1]; ++point) {
                    reduce_point(points_[point]);
                }
            } else {
                for (u32 i = 0; i < node_child_count; ++i) {
                    if (nodes_[current].children[i] == null_child_node_id) {
                        continue;
                    }
                    reduce_impl(nodes_[current].children[i], reduce_node, reduce_point, stop_condition);
                }
            }
        }
    }

    quadtree(point_container& points)
        : points_(points)
    {
    }

    void destroy_tree()
    {
        nodes_.clear();
        node_points_begin_.clear();
    }

    void build_tree()
    {
        axis_aligned_bounding_box whole_aabb = axis_aligned_bounding_box::create(points_.begin(), points_.end());
        node_id_t root_node_id               = build_impl(whole_aabb, points_.begin(), points_.end(), max_tree_depth);
        assert(root_node_id == 0);
        node_points_begin_.push_back(points_.size());
    }

    static constexpr node_id_t root_node_id = node_id_t(0);
    // Root node cannot be a child
    static constexpr node_id_t null_child_node_id = node_id_t(root_node_id);

    static constexpr u32 node_child_count = 1 << tree_dimention;

    struct node_t {
        NodeData data;
        axis_aligned_bounding_box box {};
        node_id_t children[node_child_count] {
            null_child_node_id, null_child_node_id, null_child_node_id, null_child_node_id
        };

        bool is_leaf() const noexcept
        {
            return std::all_of(
                std::begin(children), std::end(children), [](const node_id_t id) { return id == null_child_node_id; });
        }
    };

    using node_container = internal_container<node_t>;

    node_id_t
    build_impl(axis_aligned_bounding_box const& bbox, point_iterator begin, point_iterator end, u32 depth_limit)
    {
        if (begin == end) {
            return null_child_node_id;
        }

        node_id_t current_id = nodes_.size();
        nodes_.emplace_back();

        nodes_[current_id].box = bbox;

        node_points_begin_.emplace_back();
        node_points_begin_[current_id] = std::distance(points_.begin(), begin);

        if (depth_limit == 0) {
            return current_id;
        }

        if (std::equal(
                begin + 1, end, begin, [](PositionalData a, PositionalData b) { return a.position == b.position; })) {
            return current_id;
        }

        if (begin + 1 == end) {
            return current_id;
        }

        static_assert(tree_dimention == 2, "Higher dimentions does not implemented");

        point center = (bbox.min + bbox.max) / 2.0;

        auto bottom = [center](const PositionalData& p) { return p.position[1] < center[1]; };
        auto left   = [center](const PositionalData& p) { return p.position[0] < center[0]; };

        point_iterator split_y       = std::partition(begin, end, bottom);
        point_iterator split_x_lower = std::partition(begin, split_y, left);
        point_iterator split_x_upper = std::partition(split_y, end, left);

        axis_aligned_bounding_box box_0_0 = { bbox.min, center };
        nodes_[current_id].children[0]    = build_impl(box_0_0, begin, split_x_lower, depth_limit - 1);

        axis_aligned_bounding_box box_0_1 = { point { center[0], bbox.min[1] }, point { bbox.max[0], center[1] } };
        nodes_[current_id].children[1]    = build_impl(box_0_1, split_x_lower, split_y, depth_limit - 1);

        axis_aligned_bounding_box box_1_0 = { point { bbox.min[0], center[1] }, point { center[0], bbox.max[1] } };
        nodes_[current_id].children[2]    = build_impl(box_1_0, split_y, split_x_upper, depth_limit - 1);

        axis_aligned_bounding_box box_1_1 = { center, bbox.max };
        nodes_[current_id].children[3]    = build_impl(box_1_1, split_x_upper, end, depth_limit - 1);

        depth_ = std::max(max_tree_depth - depth_limit, depth_);

        return current_id;
    }

    u32 depth_ { 0 };
    point_container& points_;
    node_container nodes_;
    internal_container<u32> node_points_begin_;
};

}
