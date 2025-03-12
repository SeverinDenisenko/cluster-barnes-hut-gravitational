#include "logging.hpp"

using namespace bh;

int main()
{
    infra::setup_logging();

    LOG_INFO("Starting cluster application...");
    return 0;
}
