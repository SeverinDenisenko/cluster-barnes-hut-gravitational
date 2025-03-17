#pragma once

#include "types.hpp"

namespace bh {

class cluster_transport {
public:
    cluster_transport() = default;

    template <typename T>
    void send(array<T> msg, u32 node)
    {
        send(msg.data(), msg.size() * sizeof(T), node);
    }

    template <typename T>
    array<T> receive(u32 node)
    {
        u32 count = msg_size(node) / sizeof(T);
        array<T> msg(count);
        receive(msg.data(), count * sizeof(T), node);
        return msg;
    }

private:
    void send(void* buff, u32 size, u32 node);
    u32 msg_size(u32 node);
    void receive(void* buff, u32 size, u32 node);
};

}
