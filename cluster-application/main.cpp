#include "cluster.hpp"
#include "logging.hpp"
#include "transport.hpp"

#include "master.hpp"
#include "slave.hpp"

using namespace bh;

int main(int argc, char** argv)
{
    setup_logging();

    node this_node(argc, argv);
    cluster_transport transport;

    if (this_node.is_master()) {
        master_node master(this_node, transport);
        master.start();
    } else {
        slave_node slave(this_node, transport);
        slave.start();
    }

    return 0;
}
