#pragma once

#include "types.hpp"

namespace bh {

struct cluster_message {
    u32 magic;
};

class transport {
public:
    transport() = default;

    void send(cluster_message msg, u32 node);
    cluster_message receive(u32 node);
};

}
