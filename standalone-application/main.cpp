#include <fstream>
#include <functional>
#include <utility>
#include <vector>

#include "linalg.hpp"
#include "logging.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "tree.hpp"
#include "types.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    real dt = 0.01;
    real t0 = 0.0;
    real t  = 2 * M_PI;

    struct point_t {
        vec2 position {};
        vec2 velosity {};
        real mass {};
    };

    struct node_data_t {
        real mass {};
        vec2 mass_center {};
    };

    using nbody_quadree = quadtree<point_t, node_data_t>;

    std::vector<point_t> data
        = { point_t { .position = vec2 { -0.5, 0.0 }, .velosity = vec2 { 0.0, 0.5 }, .mass = 0.5 },
            point_t { .position = vec2 { 0.5, 0.0 }, .velosity = vec2 { 0.0, -0.5 }, .mass = 0.5 } };

    std::vector<point_t> data_copy = data;

    u32 n = data.size();

    // Preallocate nodes

    nbody_quadree tree = nbody_quadree::build(data);

    std::ofstream results("data.txt");

    while (t0 < t) {
        // Rebuild tree

        nbody_quadree::rebuild(tree);

        // Compute node masses

        tree.walk_leafs([](node_data_t& node, point_t point) { node.mass += point.mass; });

        tree.walk_nodes([](node_data_t& parent, node_data_t& child) { parent.mass += child.mass; });

        // Compute node mass centers

        tree.walk_leafs([](node_data_t& node, point_t point) {
            node.mass_center = point.position * point.mass + node.mass_center;
        });

        tree.walk_leafs(
            [](node_data_t& node, [[maybe_unused]] point_t point) { node.mass_center = node.mass_center / node.mass; });

        tree.walk_nodes([](node_data_t& parent, node_data_t& child) {
            parent.mass_center = child.mass_center * child.mass + parent.mass_center;
        });

        tree.walk_nodes([](node_data_t& parent, [[maybe_unused]] node_data_t& child) {
            parent.mass_center = parent.mass_center / parent.mass;
        });

        // Make iteration

        for (u32 i = 0; i < n; ++i) {
            vec2 acceleration { 0.0, 0.0 };

            point_t& current = tree.get_point(i);

            tree.reduce(
                [&acceleration, &current](node_data_t& node) {
                    vec2 r       = current.position - node.mass_center;
                    acceleration = acceleration - r * node.mass / std::pow(r.len(), 3);
                },
                [&acceleration, &current](point_t& point) {
                    if (point.position == current.position) {
                        return;
                    }

                    vec2 r       = current.position - point.position;
                    acceleration = acceleration - r * point.mass / std::pow(r.len(), 3);
                },
                [&data, i](nbody_quadree::axis_aligned_bounding_box aabb) -> bool {
                    return (aabb.max - aabb.min).len() / (data[i].position - aabb.max).len() < 0.1;
                });

            data_copy[i].position = current.position + current.velosity * dt + acceleration * dt * dt / 2.0;
            data_copy[i].velosity = current.velosity + acceleration * dt;

            results << fmt::format("{:+.8f} {:+.8f} {:+.8f}\n", t0, data[i].position[0], data[i].position[1]);
        }

        std::swap(data, data_copy);

        t0 += dt;
    }

    return 0;
}
