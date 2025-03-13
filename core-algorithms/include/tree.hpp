#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

// https://lisyarus.github.io/blog/posts/building-a-quadtree.html

class quadtree {
private:
    struct box_t {
        vec2 min { inf, inf };
        vec2 max { -inf, -inf };

        box_t& operator|=(vec2 const& p)
        {
            min[0] = std::min(min[0], p[0]);
            min[1] = std::min(min[1], p[1]);
            max[0] = std::max(max[0], p[0]);
            max[1] = std::max(max[1], p[1]);
            return *this;
        }

        template <typename Iterator>
        static box_t bbox(Iterator begin, Iterator end)
        {
            box_t result;
            for (auto it = begin; it != end; ++it)
                result |= *it;
            return result;
        }
    };

public:
    static quadtree build(std::vector<vec2> points)
    {
        quadtree tree;
        tree.points = std::move(points);
        tree.root   = build_impl(
            tree, box_t::bbox(tree.points.begin(), tree.points.end()), tree.points.begin(), tree.points.end(), 100);
        tree.node_points_begin.push_back(tree.points.size());
        return tree;
    }

    quadtree(const quadtree&) = delete;
    quadtree(quadtree&&)      = default;

private:
    quadtree() = default;

    using node_id_t = std::uint32_t;

    void iterate(node_id_t id, std::function<void(vec2)> do_something)
    {
        for (auto i = node_points_begin[id]; i < node_points_begin[id + 1]; ++i) {
            do_something(points[i]);
        }
    }

    static constexpr node_id_t null = node_id_t(0);
    static constexpr real inf       = std::numeric_limits<real>::infinity();

    template <typename Iterator>
    static node_id_t build_impl(quadtree& tree, box_t const& bbox, Iterator begin, Iterator end, u32 depth_limit)
    {
        if (begin == end)
            return null;

        node_id_t result = tree.nodes.size();
        tree.nodes.emplace_back();
        tree.node_points_begin.emplace_back();
        tree.node_points_begin[result] = (begin - tree.points.begin());

        if (depth_limit == 0)
            return result;

        if (std::equal(begin + 1, end, begin))
            return result;

        if (begin + 1 == end)
            return result;

        vec2 center = (bbox.min + bbox.max) / 2.0;

        auto bottom = [center](vec2 const& p) { return p[1] < center[1]; };
        auto left   = [center](vec2 const& p) { return p[0] < center[0]; };

        Iterator split_y       = std::partition(begin, end, bottom);
        Iterator split_x_lower = std::partition(begin, split_y, left);
        Iterator split_x_upper = std::partition(split_y, end, left);

        tree.nodes[result].children[0][0]
            = build_impl(tree, box_t { bbox.min, center }, begin, split_x_lower, depth_limit - 1);
        tree.nodes[result].children[0][1] = build_impl(
            tree,
            box_t { vec2 { center[0], bbox.min[1] }, vec2 { bbox.max[0], center[1] } },
            split_x_lower,
            split_y,
            depth_limit - 1);
        tree.nodes[result].children[1][0] = build_impl(
            tree,
            box_t { vec2 { bbox.min[0], center[1] }, vec2 { center[0], bbox.max[1] } },
            split_y,
            split_x_upper,
            depth_limit - 1);
        tree.nodes[result].children[1][1]
            = build_impl(tree, box_t { center, bbox.max }, split_x_upper, end, depth_limit - 1);

        return result;
    }

    struct node_t {
        box_t box;
        node_id_t children[2][2] { { null, null }, { null, null } };
    };

    box_t bbox;
    node_id_t root;
    std::vector<vec2> points;
    std::vector<node_t> nodes;
    std::vector<u32> node_points_begin;
};

}
