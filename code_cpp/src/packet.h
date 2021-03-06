#pragma once

#include <mpi.h>
#include <list>

#include "tourist.h"


class Packet {
public:
    enum msg_t {
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
        DEPART_SUBMAR_NOT_FULL,
        KILL_MESSAGE
    };

    Packet();
    Packet(msg_t message_type);
    Packet(msg_t message_type, int submar_id);
    Packet(msg_t message_type, int submar_id, int passenger_no);
    ~Packet();

    void send(Tourist &me, int destination);
    void send(Tourist &me, std::list<int> &destination_list);
    void send_to_travelling_with_me(Tourist &me);
    int broadcast(Tourist &me, int tourists_in_system);
    void receive(Tourist &me);

    msg_t get_message_type();
    int get_submarine_id();
    int get_passenger_no();
    int get_sender_id();
    int get_timestamp();
    MPI_Status get_mpi_status();

private:
    struct packet_t {
        msg_t msg_type;
        int lamport_clock;
        int submar_id;
        int passenger_no;
    };
    packet_t data_packet = {KILL_MESSAGE, 0, 0, 0};
    MPI_Status status = {0, 0, 0, 0, 0};
};
