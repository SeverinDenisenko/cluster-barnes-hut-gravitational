#include "transport.hpp"

#include <mpi.h>

namespace bh {

void cluster_transport::send(cluster_message msg, u32 node)
{
    MPI_Send(&msg, sizeof(cluster_message), MPI_BYTE, node, 0, MPI_COMM_WORLD);
}

cluster_message cluster_transport::receive(u32 node)
{
    MPI_Status status;
    cluster_message result;

    MPI_Recv(&result, sizeof(cluster_message), MPI_BYTE, node, 0, MPI_COMM_WORLD, &status);

    return result;
}

}
