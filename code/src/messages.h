#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <mpi.h>

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
    int submar_id;
    int passenger_no;
} packet_t;

inline void send_packet(packet_t *packet, int dest_id);

inline MPI_Status recv_packet(packet_t *packet);

#endif