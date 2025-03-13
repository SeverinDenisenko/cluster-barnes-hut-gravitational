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

    axis_aligned_bounding_box box_0_0 = { bbox.min, center };
    nodes_[current_id].children[0][0] = build_impl(box_0_0, begin, split_x_lower, depth_limit - 1);

    axis_aligned_bounding_box box_0_1 = { vec2 { center[0], bbox.min[1] }, vec2 { bbox.max[0], center[1] } };
    nodes_[current_id].children[0][1] = build_impl(box_0_1, split_x_lower, split_y, depth_limit - 1);

    axis_aligned_bounding_box box_1_0 = { vec2 { bbox.min[0], center[1] }, vec2 { center[0], bbox.max[1] } };
    nodes_[current_id].children[1][0] = build_impl(box_1_0, split_y, split_x_upper, depth_limit - 1);

    axis_aligned_bounding_box box_1_1 = { center, bbox.max };
    nodes_[current_id].children[1][1] = build_impl(box_1_1, split_x_upper, end, depth_limit - 1);

    return current_id;
}

}
