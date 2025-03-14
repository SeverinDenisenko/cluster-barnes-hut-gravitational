#include "tree.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace bh {

quadtree quadtree::build(point_container& points)
{
    quadtree tree(points);

    tree.build_tree();

    return tree;
}

void quadtree::rebuild(quadtree& tree)
{
    tree.destroy_tree();
    tree.build_tree();
}

void quadtree::destroy_tree()
{
    nodes_.clear();
    node_points_begin_.clear();
}

void quadtree::build_tree()
{
    axis_aligned_bounding_box whole_aabb = axis_aligned_bounding_box::create(points_.begin(), points_.end());
    node_id_t root_node_id               = build_impl(whole_aabb, points_.begin(), points_.end(), max_tree_depth);
    assert(root_node_id == 0);
    node_points_begin_.push_back(points_.size());
}

quadtree::node_id_t
quadtree::build_impl(axis_aligned_bounding_box const& bbox, point_iterator begin, point_iterator end, u32 depth_limit)
{
    if (begin == end) {
        return null_child_node_id;
    }

    node_id_t current_id = nodes_.size();
    nodes_.emplace_back();

    node_points_begin_.emplace_back();
    node_points_begin_[current_id] = std::distance(points_.begin(), begin);

    if (depth_limit == 0) {
        return current_id;
    }

    if (std::equal(
            begin + 1, end, begin, [](positional_data a, positional_data b) { return a.position == b.position; })) {
        return current_id;
    }

    if (begin + 1 == end) {
        return current_id;
    }

    static_assert(tree_dimention == 2, "Higher dimentions does not implemented");

    point center = (bbox.min + bbox.max) / 2.0;

    auto bottom = [center](const positional_data& p) { return p.position[1] < center[1]; };
    auto left   = [center](const positional_data& p) { return p.position[0] < center[0]; };

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

    return current_id;
}

void quadtree::walk_leafs(std::function<void(u32 node_id, u32 point_id)> reduce_leafs)
{
    for (node_id_t id = 0; id < nodes_.size(); ++id) {
        node_id_t node = nodes_.size() - 1 - id;

        if (!nodes_[node].is_leaf()) {
            continue;
        }

        for (u32 i = node_points_begin_[node]; i < node_points_begin_[node + 1]; ++i) {
            reduce_leafs(node, i);
        }
    }
}

void quadtree::walk_nodes(std::function<void(u32 node_id, u32 child_id)> reduce_node)
{
    for (node_id_t id = 0; id < nodes_.size(); ++id) {
        node_id_t node = nodes_.size() - 1 - id;

        if (nodes_[node].is_leaf()) {
            continue;
        }

        for (u32 i = 0; i < node_child_count; ++i) {
            if (nodes_[node].children[i] == null_child_node_id) {
                continue;
            }
            reduce_node(node, nodes_[node].children[i]);
        }
    }
}

quadtree::axis_aligned_bounding_box& quadtree::axis_aligned_bounding_box::operator|=(point const& p)
{
    min = point::min(min, p);
    max = point::max(max, p);
    return *this;
}

quadtree::axis_aligned_bounding_box
quadtree::axis_aligned_bounding_box::create(point_iterator begin, point_iterator end)
{
    axis_aligned_bounding_box result;
    for (auto it = begin; it != end; ++it)
        result |= it->position;
    return result;
}

bool quadtree::node_t::is_leaf()
{
    return std::all_of(
        std::begin(children), std::end(children), [](const node_id_t id) { return id == null_child_node_id; });
}

void quadtree::reduce(
    std::function<void(u32 node_id)> reduce_node,
    std::function<void(u32 point_id)> reduce_point,
    std::function<bool(axis_aligned_bounding_box aabb)> stop_condition)
{
    reduce_impl(root_node_id, reduce_node, reduce_point, stop_condition);
}

void quadtree::reduce_impl(
    node_id_t current,
    std::function<void(u32 node_id)> reduce_node,
    std::function<void(u32 point_id)> reduce_point,
    std::function<bool(axis_aligned_bounding_box aabb)> stop_condition)
{
    if (stop_condition(nodes_[current].box)) {
        reduce_node(current);
    } else {
        if (nodes_[current].is_leaf()) {
            for (u32 i = node_points_begin_[current]; i < node_points_begin_[current + 1]; ++i) {
                reduce_point(i);
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

}
