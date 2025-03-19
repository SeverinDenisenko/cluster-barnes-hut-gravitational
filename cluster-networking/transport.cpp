#include "transport.hpp"

#include <utility>

#include "mpi.hpp"

namespace bh {

void cluster_transport::send(void* buff, u32 size, u32 node, message_type type)
{
    MPI_Send(buff, size, MPI_BYTE, node, std::to_underlying(type), MPI_COMM_WORLD);
}

u32 cluster_transport::msg_size(u32 node, message_type type)
{
    MPI_Status status;
    int buff_size;

    MPI_Probe(node, std::to_underlying(type), MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_BYTE, &buff_size);

    return buff_size;
}

void cluster_transport::receive(void* buff, u32 size, u32 node, message_type type)
{
    MPI_Recv(buff, size, MPI_BYTE, node, std::to_underlying(type), MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

}
