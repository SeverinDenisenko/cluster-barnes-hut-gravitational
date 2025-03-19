#include "logging.hpp"

#include <sstream>
#include <string>

#include "backward.hpp"

namespace bh {

void setup_logging()
{
    spdlog::set_pattern("[%Y/%m/%d %X.%e] [%l] [P%P T%t] [%s:%#] %v");
}

std::string stacktrace()
{
    backward::StackTrace stack;
    stack.load_here();

    std::stringstream result;
    backward::Printer printer;
    printer.print(stack, result);

    return result.str();
}

}
