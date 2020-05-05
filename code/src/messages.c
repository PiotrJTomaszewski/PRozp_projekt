#include <mpi.h>
#include <pthread.h>

#include "messages.h"
#include "debug.h"

static inline int max(int a, int b) {
    return a > b ? a : b;
}

void send_packet(tourist_t *tourist, packet_t *packet, int dest_id) {
    pthread_mutex_lock(&(tourist->lamport_mutex));
    packet->lamport_clock = tourist->lamport_clock;
    tourist->lamport_clock++;
    pthread_mutex_unlock(&(tourist->lamport_mutex));
    MPI_Send(packet, sizeof(packet_t), MPI_BYTE, dest_id, 1, MPI_COMM_WORLD);
}

void broadcast_packet(tourist_t *tourist, packet_t *packet, int tourist_no) {
    for (int i=0; i<tourist_no; i++) {
        if (i != tourist->id) {
            send_packet(tourist, packet, i);
        }
    }
}

MPI_Status recv_packet(tourist_t *tourist, packet_t *packet) {
    MPI_Status status;
    MPI_Recv(packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    pthread_mutex_lock(&(tourist->lamport_mutex));
    tourist->lamport_clock = max(tourist->lamport_clock, packet->lamport_clock) + 1;
    pthread_mutex_unlock(&(tourist->lamport_mutex));
    return status;
}