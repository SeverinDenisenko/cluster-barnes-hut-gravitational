#include "transport.hpp"

#include <mpi.h>

namespace bh {

void cluster_transport::send(void* buff, u32 size, u32 node)
{
    MPI_Send(&buff, size, MPI_BYTE, node, 0, MPI_COMM_WORLD);
}

u32 cluster_transport::msg_size(u32 node)
{
    MPI_Status status;
    int buff_size;

    MPI_Probe(node, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_CHAR, &buff_size);

    return buff_size;
}

void cluster_transport::receive(void* buff, u32 size, u32 node)
{
    MPI_Status status;

    MPI_Recv(buff, size, MPI_BYTE, node, 0, MPI_COMM_WORLD, &status);
}

}
