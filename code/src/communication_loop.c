#include <mpi.h>

#include "communication_loop.h"
#include "messages.h"
#include "tourist.h"
#include "system_info.h"
#include "debug.h"

#define UNUSED(a) ((void)a)
#define NOTIMPLEMENTED(a) ((void)a)

const comm_handler_fun_t commHandlerLookup[] = {
    handler_req_pony,
    handler_ack_pony,
    handler_req_submar,
    handler_ack_submar,
    handler_full_submar_retreat,
    handler_full_submar_stay,
    handler_return_submar,
    handler_travel_ready,
    handler_ack_travel,
    handler_depart_submar,
    handler_depart_submar_not_full
};

void *communication_loop(void *data) {
    thread_data_t *thread_data = (thread_data_t*)(data);
    int run_flag = 1; // TODO: Replace with a real flag
    received_msg_data_t received_msg;
    while(run_flag) {
        received_msg.status = recv_packet(thread_data->tourist, &received_msg.msg_packet);
        pthread_mutex_lock(&(thread_data->tourist->state_mutex));
        commHandlerLookup[received_msg.msg_packet.type]((tourist_t*)(thread_data->tourist), thread_data->sys_info, &received_msg);
        pthread_mutex_unlock(&(thread_data->tourist->state_mutex));
    }
    return NULL;
}

void handler_req_pony(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    int req_sender_id = received_msg->status.MPI_SOURCE;
    debug_printf(INFO_RECEIVING, tourist, "Received REQ_PONY from %d", req_sender_id);
    packet_t packet;
    switch (tourist->state) {
        case RESTING:
        case ON_SHORE:
            packet.type = ACK_PONY;
            debug_printf(INFO_SENDING, tourist, "Sending ACK_PONY to %d", req_sender_id);
            send_packet(tourist, &packet, req_sender_id);
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
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_pony(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
            case RESTING:
            case CHOOSE_SUBMAR:
            case WAIT_SUBMAR:
            case BOARDED:
            case TRAVEL:
            case ON_SHORE:
                debug_print(INFO_RECEIVING, tourist, "Received ACK_PONY, ignoring");
                break;
            case WAIT_PONY:
                pthread_mutex_lock(&(tourist->rec_ack_cond.mutex));
                int needed_ack = sys_info->tourist_no - sys_info->pony_no;
                if (needed_ack < 0) needed_ack = 0;
                debug_printf(INFO_RECEIVING, tourist, "Received ACK_PONY, I have %d, need %d", tourist->rec_ack_no, needed_ack);
                tourist->rec_ack_no++;
                if (tourist->rec_ack_no == needed_ack) {
                    pthread_cond_signal(&(tourist->rec_ack_cond.cond));
                }
                pthread_mutex_unlock(&(tourist->rec_ack_cond.mutex));
                break;
            default:
                debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
                break;
        }
}

void handler_req_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
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
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case BOARDED:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, tourist, "Received ACK_SUBMAR, ignoring");
            break;
        case WAIT_SUBMAR:
            break;
        default:
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_full_submar_retreat(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
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
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_full_submar_stay(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
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
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_return_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
            break;
        case WAIT_PONY:
            break;
        case CHOOSE_SUBMAR:
            set_signal(&(tourist->general_cond));
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
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_travel_ready(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, tourist, "Received TRAVEL_READY, ignoring");
            break;
        case WAIT_SUBMAR:
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_ack_travel(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, tourist, "Received ACK_TRAVEL, ignoring");
            break;
        case BOARDED:
            break;
        case TRAVEL:
            break;
        default:
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_depart_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, tourist, "Received DEPART_SUBMAR, ignoring");
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}

void handler_depart_submar_not_full(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg) {
    NOTIMPLEMENTED(sys_info);
    NOTIMPLEMENTED(received_msg);
    switch (tourist->state) {
        case RESTING:
        case WAIT_PONY:
        case CHOOSE_SUBMAR:
        case WAIT_SUBMAR:
        case TRAVEL:
        case ON_SHORE:
            debug_print(INFO_RECEIVING, tourist, "Received DEPART_SUBMAR_NOT_FULL, ignoring");
            break;
        case BOARDED:
            break;
        default:
            debug_print(ERROR_RECEIVING, tourist, "Error, What?! I'm in a wrong state");
            break;
    }
}