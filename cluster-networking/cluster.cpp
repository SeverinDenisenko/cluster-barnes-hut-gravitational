#include "cluster.hpp"

#include <mpi.h>

namespace bh {

node::node(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nodes_count_);
    MPI_Comm_rank(MPI_COMM_WORLD, &node_index_);

    if (node_index_ == 0) {
        is_master_ = true;
    } else {
        is_master_ = false;
    }
}

node::~node()
{
    MPI_Finalize();
}

}
