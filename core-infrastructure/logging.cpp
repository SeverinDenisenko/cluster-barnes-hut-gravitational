#include "logging.hpp"

namespace bh {

void setup_logging()
{
    spdlog::set_pattern("[%Y/%m/%d %X.%e] [%l] [T%t] [%s:%#] %v");
}

}
