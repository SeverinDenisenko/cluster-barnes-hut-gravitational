#pragma once

#include <functional>

namespace bh {

class unit { };

template <typename Ret, typename Arg>
using task_t = std::function<Ret(Arg)>;

};
