#pragma once

#include <vector>

#include "model.hpp"

namespace bh {

struct generator_params {
    u32 count;
};

class generator {
public:
    generator(generator_params params);

    std::vector<point_t> generate();

private:
    generator_params params_;
};

}
