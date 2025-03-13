#include "logging.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting cluster application...");
    return 0;
}
