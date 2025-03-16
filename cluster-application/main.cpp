#include "cluster.hpp"
#include "logging.hpp"

using namespace bh;

void master_main(transport&)
{
    LOG_INFO("Starting main application...");
}

void slave_main(transport&)
{
    LOG_INFO("Starting slave application...");
}

int main(int argc, char** argv)
{
    setup_logging();

    node this_node(argc, argv);
    transport transport;

    if (this_node.is_master()) {
        master_main(transport);
    } else {
        slave_main(transport);
    }

    return 0;
}
