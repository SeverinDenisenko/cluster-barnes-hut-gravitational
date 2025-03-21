#include "cluster.hpp"

#include "mpi.hpp"

namespace bh {

node::node(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nodes_count_);
    MPI_Comm_rank(MPI_COMM_WORLD, &node_index_);
}

node::~node()
{
    MPI_Finalize();
}

}
