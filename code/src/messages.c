#include <mpi.h>
#include <pthread.h>

#include "messages.h"
#include "debug.h"
#include "shared_var.h"
#include "tourist.h"

static inline int max(int a, int b) {
    return a > b ? a : b;
}

void send_packet(tourist_t *tourist, packet_t *packet, int dest_id) {
     packet->lamport_clock = increment_shared_int(&(tourist->lamport_clock));
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
    pthread_mutex_lock(&(tourist->lamport_clock.mutex));
    tourist->lamport_clock.value = max(tourist->lamport_clock.value, packet->lamport_clock) + 1;
    pthread_mutex_unlock(&(tourist->lamport_clock.mutex));
    return status;
}

void send_packet_to_travelling_with_me(tourist_t *tourist, packet_t *packet) {
    int other_id;
    for (int i=0; i<stack_get_size(&(tourist->travelling_with_me)); i++) {
                other_id = stack_get_value_at(&(tourist->travelling_with_me), i);
                if (other_id == tourist->id) continue; // Don't send message to yourself
                send_packet(tourist, packet, other_id);
    }
}
