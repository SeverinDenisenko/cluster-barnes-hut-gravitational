#include "logging.hpp"

namespace bh::infra {

void setup_logging()
{
    spdlog::set_pattern("[%Y/%m/%d %X.%e] [%l] [T%t] [%s:%#] %v");
}

}
