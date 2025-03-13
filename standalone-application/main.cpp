#include <functional>
#include <vector>

#include "linalg.hpp"
#include "logging.hpp"
#include "tree.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    std::vector<vec2> pos  = { vec2 { -1.0, 0.0 }, vec2 { 1.0, 0.0 } };
    std::vector<vec2> vel  = { vec2 { 0.0, 1.0 }, vec2 { 0.0, -1.0 } };
    std::vector<vec2> acc  = { vec2 { 0.0, 0.0 }, vec2 { 0.0, 0.0 } };
    std::vector<real> mass = { 1.0, 2.0 };

    quadtree tree = quadtree::build(pos);

    std::vector<real> node_mass(tree.node_count());
    std::vector<vec2> node_mass_center(tree.node_count());

    // Compute node masses

    tree.walk_leafs([&node_mass, &mass](u32 node, u32 point) { node_mass[node] += mass[point]; });

    tree.walk_nodes([&node_mass](u32 parent, u32 child) { node_mass[parent] += node_mass[child]; });

    // Compute node mass centers

    tree.walk_leafs([&node_mass_center, &mass, &pos](u32 node, u32 point) {
        node_mass_center[node] = pos[point] * mass[point] + node_mass_center[node];
    });

    tree.walk_leafs([&node_mass_center, &node_mass](u32 node, [[maybe_unused]] u32 point) {
        node_mass_center[node] = node_mass_center[node] / node_mass[node];
    });

    tree.walk_nodes([&node_mass_center, &node_mass](u32 parent, u32 child) {
        node_mass_center[parent] = node_mass_center[child] * node_mass[child] + node_mass_center[parent];
    });

    tree.walk_nodes([&node_mass_center, &node_mass](u32 parent, [[maybe_unused]] u32 child) {
        node_mass_center[parent] = node_mass_center[parent] / node_mass[parent];
    });

    // Make iteration

    return 0;
}
