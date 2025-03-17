#include "cluster.hpp"
#include "logging.hpp"
#include "transport.hpp"

using namespace bh;

void master_main(node& node, transport& transport)
{
    LOG_INFO("Starting main application...");

    cluster_message request { .magic = node.node_index() };

    for (u32 i = 1; i < node.nodes_count(); ++i) {
        transport.send(request, i);

        cluster_message response = transport.receive(i);

        LOG_INFO(response.magic);
    }

    LOG_INFO("Exiting main application...");
}

void slave_main(node& node, transport& transport)
{
    LOG_INFO("Starting slave application...");

    cluster_message response { .magic = node.node_index() };

    cluster_message msg = transport.receive(node.master_node_index());

    LOG_INFO(msg.magic);

    transport.send(response, node.master_node_index());

    LOG_INFO("Exiting slave application...");
}

int main(int argc, char** argv)
{
    setup_logging();

    node this_node(argc, argv);
    transport transport;

    if (this_node.is_master()) {
        master_main(this_node, transport);
    } else {
        slave_main(this_node, transport);
    }

    return 0;
}
