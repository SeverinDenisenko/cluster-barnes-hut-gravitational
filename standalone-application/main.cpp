#include "logging.hpp"
#include "tree.hpp"

using namespace bh;

int main()
{
    setup_logging();

    LOG_INFO("Starting standalone application...");

    return 0;
}
