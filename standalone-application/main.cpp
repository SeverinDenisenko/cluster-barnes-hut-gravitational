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

    tree.walk([&node_mass, &mass](u32 node, u32 point) { node_mass[node] += mass[point]; });

    tree.walk([&node_mass](u32 parent, u32 child_a, u32 child_b, u32 child_c, u32 child_d) {
        node_mass[parent] = node_mass[child_a] + node_mass[child_b] + node_mass[child_c] + node_mass[child_d];
    });

    // Compute node mass centers

    tree.walk([&node_mass_center, &mass, &pos](u32 node, u32 point) {
        node_mass_center[node] = pos[point] * mass[point] + node_mass_center[node];
    });

    tree.walk([&node_mass_center, &node_mass](u32 node, [[maybe_unused]] u32 point) {
        node_mass_center[node] = node_mass_center[node] / node_mass[node];
    });

    tree.walk([&node_mass_center, &node_mass](u32 parent, u32 child_a, u32 child_b, u32 child_c, u32 child_d) {
        node_mass_center[parent] = node_mass_center[child_a] * node_mass[child_a]
            + node_mass_center[child_b] * node_mass[child_b] + node_mass_center[child_c] * node_mass[child_c]
            + node_mass_center[child_d] * node_mass[child_d];
        node_mass_center[parent] = node_mass_center[parent] / node_mass[parent];
    });

    return 0;
}
