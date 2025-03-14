#include <chrono>
#include <fstream>
#include <functional>
#include <random>
#include <utility>
#include <vector>

#include "linalg.hpp"
#include "logging.hpp"
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

    u32 n = 10000;

    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(-1.0, 1.0);

    std::vector<point_t> data;

    for (u32 i = 0; i < n; ++i) {
        data.push_back(point_t { .position = vec2 { dist(e2), dist(e2) },
                                 .velosity = vec2 { dist(e2) / n, dist(e2) / n },
                                 .mass     = dist(e2) / n });
    }

    std::vector<point_t> data_copy = data;

    LOG_INFO("Starting calculation...");

    // Preallocate nodes

    nbody_quadree tree = nbody_quadree::build(data);

    struct stats {
        u32 point_hit {};
        u32 node_hit {};
    };

    struct time_stats {
        using clock = std::chrono::high_resolution_clock;

        f32 tree_build_time {};
        f32 acceleration_calculation_time {};
    };

    stats hit_stats {};
    time_stats time_stats;

    while (t0 < t) {
        // Rebuild tree

        auto now = time_stats::clock::now();

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

        time_stats.tree_build_time
            += std::chrono::duration_cast<std::chrono::milliseconds>(time_stats::clock::now() - now).count();
        now = time_stats::clock::now();

        // Make iteration

        for (u32 i = 0; i < n; ++i) {
            vec2 acceleration { 0.0, 0.0 };

            point_t& current = tree.get_point(i);

            tree.reduce(
                [&acceleration, &current, &hit_stats](node_data_t& node) {
                    vec2 r       = current.position - node.mass_center;
                    acceleration = acceleration - r * node.mass / std::pow(r.len(), 3);

                    hit_stats.node_hit += 1;
                },
                [&acceleration, &current, &hit_stats](point_t& point) {
                    if (point.position == current.position) {
                        return;
                    }

                    vec2 r       = current.position - point.position;
                    acceleration = acceleration - r * point.mass / std::pow(r.len(), 3);

                    hit_stats.point_hit += 1;
                },
                [&data, i](nbody_quadree::axis_aligned_bounding_box aabb) -> bool {
                    return (aabb.max - aabb.min).len() / (data[i].position - aabb.max).len() < 0.1;
                });

            data_copy[i].position = current.position + current.velosity * dt + acceleration * dt * dt / 2.0;
            data_copy[i].velosity = current.velosity + acceleration * dt;
        }

        time_stats.acceleration_calculation_time
            += std::chrono::duration_cast<std::chrono::milliseconds>(time_stats::clock::now() - now).count();
        now = time_stats::clock::now();

        std::swap(data, data_copy);

        f32 summary_hits = hit_stats.point_hit + hit_stats.node_hit;
        LOG_INFO(fmt::format(
            "Stats: nodes={}, points_evaluations={:.1f}%, nodes_evaluations={:.1f}%",
            tree.node_count(),
            hit_stats.point_hit / summary_hits * 100.0,
            hit_stats.node_hit / summary_hits * 100.0));

        f32 summary_time = time_stats.tree_build_time + time_stats.acceleration_calculation_time;
        LOG_INFO(fmt::format(
            "Time stats: tree_build_time={:.1f}%, iteration_calculation_time={:.1f}%",
            time_stats.tree_build_time / summary_time * 100.0,
            time_stats.acceleration_calculation_time / summary_time * 100.0));

        t0 += dt;
    }

    LOG_INFO("Saving results...");

    std::ofstream results("data.txt");
    for (u32 i = 0; i < n; ++i) {
        results << fmt::format("{:+.8f} {:+.8f} {:+.8f}\n", t0, data[i].position[0], data[i].position[1]);
    }

    LOG_INFO("Exiting...");

    return 0;
}
