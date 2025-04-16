#include "transport.hpp"

#include "mpi.hpp"

namespace bh {

void cluster_transport::send(void* buff, u32 size, u32 node, u32 type)
{
    MPI_Send(buff, size, MPI_BYTE, node, type, MPI_COMM_WORLD);
}

u32 cluster_transport::msg_size(u32 node, u32 type)
{
    MPI_Status status;
    int buff_size;

    MPI_Probe(node, type, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_BYTE, &buff_size);

    return buff_size;
}

bool cluster_transport::can_recive(u32 node, u32 msg_id)
{
    MPI_Status status;
    int flag;

    MPI_Iprobe(node, msg_id, MPI_COMM_WORLD, &flag, &status);

    return static_cast<bool>(flag);
}

void cluster_transport::receive(void* buff, u32 size, u32 node, u32 type)
{
    MPI_Recv(buff, size, MPI_BYTE, node, type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

}
