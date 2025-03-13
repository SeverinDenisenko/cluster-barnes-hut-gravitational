#pragma once

#include <functional>
#include <vector>

#include "linalg.hpp"
#include "types.hpp"

namespace bh {

// Barnes-Hut tree does not own points or other data, it's only contains structure of the tree

class quadtree {
public:
    template <typename T>
    using internal_container = std::vector<T>;
    template <typename T>
    using internal_interator = internal_container<T>::iterator;

    using point_container = internal_container<vec2>;
    using point_iterator  = internal_interator<vec2>;

    static constexpr u32 max_tree_depth = 100;

    static quadtree build(point_container& points);

    static void rebuild(quadtree& tree);

    quadtree(const quadtree&) = delete;
    quadtree(quadtree&&)      = default;

    u32 node_count() const
    {
        return nodes_.size();
    }

private:
    quadtree(point_container& points)
        : points_(points)
    {
    }

    void destroy_tree();

    void build_tree();

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

    node_id_t
    build_impl(axis_aligned_bounding_box const& bbox, point_iterator begin, point_iterator end, u32 depth_limit);

    point_container& points_;
    internal_container<node_t> nodes_;
    internal_container<u32> node_points_begin_;
};

}
