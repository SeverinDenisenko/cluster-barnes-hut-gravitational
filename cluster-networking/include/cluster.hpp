#pragma once

#include "types.hpp"

namespace bh {

class node {
public:
    node(int argc, char** argv);
    ~node();

    bool is_master() const noexcept
    {
        return is_master_;
    }

    u32 nodes_count() const noexcept
    {
        return nodes_count_;
    }

    u32 node_index() const noexcept
    {
        return node_index_;
    }

    u32 master_node_index() const noexcept
    {
        return 0;
    }

private:
    int node_index_;
    int nodes_count_;
    bool is_master_;
};

}
