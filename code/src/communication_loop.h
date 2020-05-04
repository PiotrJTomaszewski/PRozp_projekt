#ifndef __COMMUNICATION_LOOP_H__
#define __COMMUNICATION_LOOP_H__

#include <pthread.h>
#include <mpi.h>

#include "system_info.h"
#include "tourist.h"
#include "messages.h"

typedef struct ThreadData {
    tourist_t *tourist;
    system_info_t *sys_info;
} thread_data_t;

void *communication_loop(void *data);

void handler_req_pony(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_ack_pony(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_req_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_ack_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_full_submar_retreat(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_full_submar_stay(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_return_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_travel_ready(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_ack_travel(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_depart_submar(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);
void handler_depart_submar_not_full(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);

typedef void (*comm_handler_fun_t)(tourist_t *tourist, system_info_t *sys_info, received_msg_data_t *received_msg);

extern const comm_handler_fun_t commHandlerLookup[];

#endif