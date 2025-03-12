#pragma once

#include "spdlog/spdlog.h"

#define LOG_INFO SPDLOG_INFO
#define LOG_DEBUG SPDLOG_DEBUG
#define LOG_TRACE SPDLOG_TRACE

namespace bh::infra {

void setup_logging();

}
