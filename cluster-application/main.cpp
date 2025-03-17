#include "cluster.hpp"
#include "ev_loop.hpp"
#include "generator.hpp"
#include "logging.hpp"
#include "model.hpp"
#include "transport.hpp"

using namespace bh;

void master_main(ev_loop& loop, const node& node)
{
    LOG_INFO("Starting main application...");

    cluster_transport transport;

    array<point_t> points = generator { generator_params { .count = 1'000 } }.generate();

    LOG_INFO(points.size());

    for (u32 i : node.slaves_node_indexes()) {
        transport.send(points, i);
    }

    loop.stop();

    LOG_INFO("Exiting main application...");
}

void slave_main(ev_loop& loop, const node& node)
{
    LOG_INFO("Starting slave application...");

    cluster_transport transport;

    array<point_t> points = transport.receive<point_t>(node.master_node_index());

    LOG_INFO(points.size());

    loop.stop();

    LOG_INFO("Exiting slave application...");
}

int main(int argc, char** argv)
{
    setup_logging();

    node this_node(argc, argv);

    ev_loop loop;

    loop.start([&this_node, &loop]() {
        if (this_node.is_master()) {
            master_main(loop, this_node);
        } else {
            slave_main(loop, this_node);
        }
    });

    loop.join();

    return 0;
}
