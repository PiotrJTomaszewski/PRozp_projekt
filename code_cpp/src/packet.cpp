#include "packet.h"
#include "shared_var.h"

Packet::Packet() {}

Packet::Packet(msg_t message_type) {
    msg_type = message_type;
}

Packet::Packet(msg_t message_type, int submar_id) {
    msg_type = message_type;
    data_packet.submar_id = submar_id;
}

Packet::Packet(msg_t message_type, int submar_id, int passenger_no) {
    msg_type = message_type;
    data_packet.submar_id = submar_id;
    data_packet.passenger_no = passenger_no;
}

Packet::~Packet() {}

void Packet::send(Tourist &me, int destination) {
    data_packet.lamport_clock = me.lamport_clock++;
    MPI_Send(&data_packet, sizeof(packet_t), MPI_BYTE, destination, static_cast<int>(msg_type), MPI_COMM_WORLD);
}

void Packet::send(Tourist &me, std::list<int> &destination_list) {
    data_packet.lamport_clock = me.lamport_clock++;
    for (auto it = destination_list.begin(); it != destination_list.end(); ++it) {
        MPI_Send(&data_packet, sizeof(packet_t), MPI_BYTE, *it, static_cast<int>(msg_type), MPI_COMM_WORLD);
    }
}

void Packet::send_to_travelling_with_me(Tourist &me) {
    int my_id = me.get_id();
    int passenger_id;
    data_packet.lamport_clock = me.lamport_clock++;
    me.boarded_on_my_submarine.mutex_lock();
    for (int position = 0; position < me.boarded_on_my_submarine.unsafe_get_size(); position++) {
        passenger_id = me.boarded_on_my_submarine.unsafe_get_element(position);
        if (passenger_id == my_id)
            continue;
        MPI_Send(&data_packet, sizeof(packet_t), MPI_BYTE, passenger_id, static_cast<int>(msg_type), MPI_COMM_WORLD);
    }
    me.boarded_on_my_submarine.mutex_unlock();
}

int Packet::broadcast(Tourist &me, int tourists_in_system) {
    int my_id = me.get_id();
    data_packet.lamport_clock = me.lamport_clock++; // TODO: Should increment after every message or just once?
    for (int id = 0; id < tourists_in_system; id++) {
        if (id != my_id) {
            MPI_Send(&data_packet, sizeof(packet_t), MPI_BYTE, id, static_cast<int>(msg_type), MPI_COMM_WORLD);
        }
    }
    return data_packet.lamport_clock;
}

void Packet::receive(Tourist &me) {
    MPI_Recv(&data_packet, sizeof(packet_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    msg_type = static_cast<msg_t>(status.MPI_TAG);
    me.lamport_clock.store(std::max(me.lamport_clock.load(), data_packet.lamport_clock) + 1);
}

Packet::msg_t Packet::get_message_type() {
    return msg_type;
}

int Packet::get_submarine_id() {
    return data_packet.submar_id;
}

int Packet::get_passenger_no() {
    return data_packet.passenger_no;
}

int Packet::get_sender_id() {
    return status.MPI_SOURCE;
}

int Packet::get_timestamp() {
    return data_packet.lamport_clock;
}
