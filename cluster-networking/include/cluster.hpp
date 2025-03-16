#pragma once

namespace bh {

class node {
public:
    node(int argc, char** argv);
    ~node();

    bool is_master() const noexcept
    {
        return is_master_;
    }

private:
    int node_index_;
    int nodes_count_;
    bool is_master_;
};

class transport {
public:
    transport() = default;
};

}
