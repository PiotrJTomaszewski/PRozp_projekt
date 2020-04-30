#include <mpi.h>

#include "communication_loop.h"
#include "messages.h"
#include "tourist.h"
#include "system_info.h"
#include "debug.h"

void communication_loop(thread_data_t *thread_data) {
    int run_flag = 1; // TODO: Replace with a real flag
    volatile comm_loop_data_t comm_data;
    comm_data.sys_info = thread_data->sys_info;
    comm_data.tourist = thread_data->tourist;
    while(run_flag) {
        comm_data.status = recv_packet(&(comm_data.msg_packet));
        handlerLookup[comm_data.msg_packet.type];
    }
}

void handler_req_pony(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id,
                data->tourist->state, "Received REQ_PONY, answering");
            packet_t packet;
            packet.type = ACK_PONY;
            send_packet(&packet, data->status.MPI_SOURCE);
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                    data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_pony(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
            case RESTING:
            case CHOOSE_SUBMAR:
            case WAIT_SUBMAR:
            case BOARDED:
            case TRAVEL:
                debug_print(INFO_RECEIVING, data->tourist->id,
                    data->tourist->state, "Received ACK_PONY, ignoring");
                break;
            case WAIT_PONY:
                break;
            case ON_SHORE:
                break;
            default:
                debug_print(ERROR_RECEIVING, data->tourist->id,
                    data->tourist->state, "Error, What?! I'm in a wrong state");
                break;
        }
}

void handler_req_submar(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        case ON_SHORE:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_submar(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case BOARDED:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id, 
                data->tourist->state, "Received ACK_SUBMAR, ignoring");
            break;
        case WAIT_SUBMAR:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_full_submar_retreat(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        case ON_SHORE:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_full_submar_stay(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        case ON_SHORE:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_return_submar(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        case ON_SHORE:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_travel_ready(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id,
                data->tourist->state, "Received TRAVEL_READY, ignoring");
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_travel(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id,
                data->tourist->state, "Received ACK_TRAVEL, ignoring");
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_depart_submar(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id,
                data->tourist->state, "Received DEPART_SUBMAR, ignoring");
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_depart_submar_not_full(volatile comm_loop_data_t *data) {
    switch (data->tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, data->tourist->id,
                data->tourist->state, "Received DEPART_SUBMAR_NOT_FULL, ignoring");
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, data->tourist->id,
                data->tourist->state, "Error, What?! I'm in a wrong state");
            break;
    }
}