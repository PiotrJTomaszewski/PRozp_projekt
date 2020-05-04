#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "main_loop.h"
#include "messages.h"
#include "debug.h"

#define UNUSED(a) ((void)a)
#define NOTIMPLEMENTED(a) ((void)a)
#define MAX_REST_TIME 20

const main_handler_fun_t mainHandlerLookup[] = {
    handler_resting,
    handler_wait_pony,
    handler_choose_submar,
    handler_boarded,
    handler_travel,
    handler_on_shore
};

void main_loop(tourist_t *tourist, system_info_t *sys_info) {
    srand(time(NULL));
    int run_flag = 1; // TODO: Replace with a real flag
    while(run_flag) {
        mainHandlerLookup[tourist->state](tourist, sys_info);
    }
}

void handler_resting(tourist_t *tourist, system_info_t *sys_info) {
    int resting_time = rand() % MAX_REST_TIME;
    debug_printf(INFO_OTHER, tourist, "Going to sleep for %d seconds", resting_time);
    sleep(resting_time);
    debug_printf(INFO_SENDING, tourist, "Woke up. Requesting a pony suit");
    packet_t packet;
    packet.type = REQ_PONY;
    pthread_mutex_lock(&(tourist->state_mutex));  // Locking to avoid ignoring ACK if they came before the state is changed
    broadcast_packet(tourist, &packet, sys_info->tourist_no);
    debug_print(INFO_CHANGE_STATE, tourist, "Changing state to WAIT_PONY");
    tourist->state = WAIT_PONY;
    pthread_mutex_unlock(&(tourist->state_mutex));
}

void handler_wait_pony(tourist_t *tourist, system_info_t *sys_info) {
    UNUSED(sys_info);
    debug_print(INFO_OTHER, tourist, "Waiting for enough ACK_PONY");
    wait_for_signal(tourist);
    debug_print(INFO_CHANGE_STATE, tourist, "Received enough ACK_PONY, getting a suit and moving to the next state");
    change_state(tourist, CHOOSE_SUBMAR);
}

void handler_choose_submar(tourist_t *tourist, system_info_t *sys_info) {
    // Choosing the best submarine
    int submar_id = get_best_submarine(tourist, sys_info);
    if (submar_id >= 0) {
        debug_printf(INFO_OTHER, tourist, "I've chosen a submarine with id %d, asking for permission", submar_id);
        packet_t packet;
        packet.type = REQ_SUBMAR;
        packet.submar_id = submar_id;
        pthread_mutex_lock(&(tourist->state_mutex));
        broadcast_packet(tourist, &packet, sys_info->tourist_no);
        debug_print(INFO_CHANGE_STATE, tourist, "Changing state to WAIT_SUBMAR");
        tourist->state = WAIT_SUBMAR;
        pthread_mutex_unlock(&(tourist->state_mutex));
    } else {
        debug_print(INFO_OTHER, tourist, "No free submarine found, waiting for one to return");
        wait_for_signal(tourist);
        debug_print(INFO_OTHER, tourist, "I've noticed that a submarine has returned, waking up");
    }
}

void handler_wait_submar(tourist_t *tourist, system_info_t *sys_info) {
    NOTIMPLEMENTED(tourist);
    NOTIMPLEMENTED(sys_info);
}

void handler_boarded(tourist_t *tourist, system_info_t *sys_info) {
    NOTIMPLEMENTED(tourist);
    NOTIMPLEMENTED(sys_info);
}

void handler_travel(tourist_t *tourist, system_info_t *sys_info) {
    NOTIMPLEMENTED(tourist);
    NOTIMPLEMENTED(sys_info);
}

void handler_on_shore(tourist_t *tourist, system_info_t *sys_info) {
    NOTIMPLEMENTED(tourist);
    NOTIMPLEMENTED(sys_info);
}