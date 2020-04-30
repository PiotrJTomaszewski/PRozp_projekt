#ifndef __COMMUNICATION_LOOP_H__
#define __COMMUNICATION_LOOP_H__

#include <pthread.h>
#include <mpi.h>

#include "system_info.h"
#include "tourist.h"
#include "messages.h"

typedef struct ThreadData {
    system_info_t *sys_info;
    tourist_t *tourist;
    pthread_mutex_t *tourist_mutex;
} thread_data_t;

typedef struct CommLoopData {
    packet_t msg_packet;
    MPI_Status status;
    system_info_t *sys_info;
    tourist_t *tourist;
    pthread_mutex_t *tourist_mutex;
} comm_loop_data_t;

void communication_loop(thread_data_t *data);

void handler_req_pony(volatile comm_loop_data_t *data);
void handler_ack_pony(volatile comm_loop_data_t *data);
void handler_req_submar(volatile comm_loop_data_t *data);
void handler_ack_submar(volatile comm_loop_data_t *data);
void handler_full_submar_retreat(volatile comm_loop_data_t *data);
void handler_full_submar_stay(volatile comm_loop_data_t *data);
void handler_return_submar(volatile comm_loop_data_t *data);
void handler_travel_ready(volatile comm_loop_data_t *data);
void handler_ack_travel(volatile comm_loop_data_t *data);
void handler_depart_submar(volatile comm_loop_data_t *data);
void handler_depart_submar_not_full(volatile comm_loop_data_t *data);

typedef void (*handler_fun_t)(volatile comm_loop_data_t *data);

handler_fun_t handlerLookup[] = {
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
#endif