#include <mpi.h>

#include "messages.h"

inline void send_packet(packet_t *packet, int dest_id) {
    MPI_Send(packet, sizeof(packet_t), MPI_BYTE, dest_id, MPI_ANY_TAG, MPI_COMM_WORLD);
}

inline MPI_Status recv_packet(packet_t *packet) {
    MPI_Status status;
    MPI_Recv(packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    return status;
}