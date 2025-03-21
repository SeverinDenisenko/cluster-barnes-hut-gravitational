#pragma once

#include <numeric>

#include "types.hpp"

namespace bh {

class node {
public:
    node(int argc, char** argv);
    ~node();

    node(const node&) = delete;
    node(node&&)      = delete;

    bool is_master() const noexcept
    {
        if (node_index_ == 0) {
            return true;
        } else {
            return false;
        }
    }

    bool is_frontend() const noexcept
    {
        if (node_index_ == 1) {
            return true;
        } else {
            return false;
        }
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

    u32 frontend_node_index() const noexcept
    {
        return 1;
    }

    array<u32> slaves_node_indexes() const noexcept
    {
        array<u32> slaves(nodes_count_ - 2);
        std::iota(std::begin(slaves), std::end(slaves), 2);
        return slaves;
    }

private:
    int node_index_;
    int nodes_count_;
};

}
