#include <fstream>
#include <functional>
#include <utility>
#include <vector>

#include "linalg.hpp"
#include "logging.hpp"
#include "spdlog/fmt/bundled/format.h"
#include "tree.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    real dt = 0.01;
    real t0 = 0.0;
    real t  = 2 * M_PI;

    std::vector<quadtree::positional_data> data
        = { quadtree::positional_data { .position = vec2 { -0.5, 0.0 }, .velosity = vec2 { 0.0, 0.5 }, .mass = 0.5 },
            quadtree::positional_data { .position = vec2 { 0.5, 0.0 }, .velosity = vec2 { 0.0, -0.5 }, .mass = 0.5 } };

    std::vector<quadtree::positional_data> data_copy = data;

    u32 n = data.size();

    // Preallocate nodes

    quadtree tree = quadtree::build(data);

    struct node_data_t {
        real mass {};
        vec2 mass_center {};
    };

    std::ofstream results("data.txt");

    while (t0 < t) {
        // Rebuild tree

        quadtree::rebuild(tree);

        std::vector<node_data_t> node_data(tree.node_count());

        // Compute node masses

        tree.walk_leafs([&node_data, &data](u32 node, u32 point) { node_data[node].mass += data[point].mass; });

        tree.walk_nodes([&node_data](u32 parent, u32 child) { node_data[parent].mass += node_data[child].mass; });

        // Compute node mass centers

        tree.walk_leafs([&node_data, &data](u32 node, u32 point) {
            node_data[node].mass_center = data[point].position * data[point].mass + node_data[node].mass_center;
        });

        tree.walk_leafs([&node_data](u32 node, [[maybe_unused]] u32 point) {
            node_data[node].mass_center = node_data[node].mass_center / node_data[node].mass;
        });

        tree.walk_nodes([&node_data](u32 parent, u32 child) {
            node_data[parent].mass_center
                = node_data[child].mass_center * node_data[child].mass + node_data[parent].mass_center;
        });

        tree.walk_nodes([&node_data](u32 parent, [[maybe_unused]] u32 child) {
            node_data[parent].mass_center = node_data[parent].mass_center / node_data[parent].mass;
        });

        // Make iteration

        for (u32 i = 0; i < n; ++i) {
            vec2 acc { 0.0, 0.0 };

            tree.reduce(
                [&acc, &node_data, &data, i](u32 node) {
                    vec2 r = data[i].position - node_data[node].mass_center;
                    acc    = acc - r * node_data[node].mass / std::pow(r.len(), 3);
                },
                [&acc, &data, i](u32 point) {
                    if (point == i) {
                        return;
                    }

                    vec2 r = data[i].position - data[point].position;
                    acc    = acc - r * data[point].mass / std::pow(r.len(), 3);
                },
                [&data, i](quadtree::axis_aligned_bounding_box aabb) -> bool {
                    return (aabb.max - aabb.min).len() / (data[i].position - aabb.max).len() < 0.1;
                });

            data_copy[i].position = data[i].position + data[i].velosity * dt + acc * dt * dt / 2.0;
            data_copy[i].velosity = data[i].velosity + acc * dt;

            results << fmt::format("{:+.8f} {:+.8f} {:+.8f}\n", t0, data[i].position[0], data[i].position[1]);
        }

        std::swap(data, data_copy);

        t0 += dt;
    }

    return 0;
}
