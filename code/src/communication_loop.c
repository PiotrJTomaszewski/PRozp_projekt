#include <mpi.h>

#include "communication_loop.h"
#include "messages.h"
#include "tourist.h"
#include "system_info.h"

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
            break;
    }
}

void handler_ack_pony(volatile comm_loop_data_t *data) {
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
            break;
    }
}

void handler_ack_submar(volatile comm_loop_data_t *data) {
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
            break;
    }
}

void handler_travel_ready(volatile comm_loop_data_t *data) {
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
            break;
    }
}

void handler_ack_travel(volatile comm_loop_data_t *data) {
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
            break;
    }
}

void handler_depart_submar(volatile comm_loop_data_t *data) {
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
            break;
    }
}

void handler_depart_submar_not_full(volatile comm_loop_data_t *data) {
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
            break;
    }
}