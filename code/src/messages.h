#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <mpi.h>

#include "tourist.h"

typedef enum MessageType {
    REQ_PONY,
    ACK_PONY,
    REQ_SUBMAR, // {submar_id}
    ACK_SUBMAR,
    FULL_SUBMAR_RETREAT, // {submar_id}
    FULL_SUBMAR_STAY, // {submar_id}
    RETURN_SUBMAR, // {submar_id, passenger_no}
    TRAVEL_READY,
    ACK_TRAVEL,
    DEPART_SUBMAR,
    DEPART_SUBMAR_NOT_FULL
} msg_t;

typedef struct Packet {
    msg_t type;
    int lamport_clock;
    int submar_id;
    int passenger_no;
} packet_t;

typedef struct ReceivedMsgData {
    packet_t msg_packet;
    MPI_Status status;
} received_msg_data_t;

// Lamport clock is set automatically
void send_packet(tourist_t *tourist, packet_t *packet, int dest_id);

// Lamport clock is set automatically
void broadcast_packet(tourist_t *tourist, packet_t *packet, int tourist_no);

MPI_Status recv_packet(tourist_t *tourist, packet_t *packet);

#endif