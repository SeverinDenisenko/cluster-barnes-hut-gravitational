#include "logging.hpp"

using namespace bh;

int main()
{
    infra::setup_logging();

    LOG_INFO("Starting standalone application...");
    return 0;
}
