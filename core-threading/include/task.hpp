#pragma once

#include <functional>
#include <stdexcept>
#include <type_traits>

namespace bh {

class unit { };

template <typename Ret, typename Arg>
using task_t = std::function<Ret(Arg)>;

};
