#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "main_loop.h"
#include "messages.h"
#include "debug.h"
#include "cond_signal.h"

#define UNUSED(a) ((void)a)
#define NOTIMPLEMENTED(a) ((void)a)

const main_handler_fun_t mainHandlerLookup[] = {
    handler_resting,
    handler_wait_pony,
    handler_choose_submar,
    handler_boarded,
    handler_travel,
    handler_on_shore
};

void main_loop(tourist_t *tourist, system_info_t *sys_info) {
    srand(tourist->id);
    int run_flag = 1; // TODO: Replace with a real flag
    while(run_flag) {
        mainHandlerLookup[tourist->state](tourist, sys_info);
    }
}

void handler_resting(tourist_t *tourist, system_info_t *sys_info) {
    int resting_time = rand() % sys_info->max_rest_time + sys_info->min_rest_time;
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
    int needed_ack = sys_info->tourist_no - sys_info->pony_no;
    if (needed_ack <= 1) {
        debug_print(INFO_CHANGE_STATE, tourist, "Don't need any ACK_PONY, so I'm taking a suit");
    } else {
        debug_print(INFO_WAITING, tourist, "Waiting for enough ACK_PONY");
        wait_for_signal(&(tourist->rec_ack_cond));
        debug_print(INFO_CHANGE_STATE, tourist, "Received enough ACK_PONY, getting a suit and moving to the next state");
    }
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
        tourist->my_submarine = submar_id;
        pthread_mutex_lock(&(tourist->state_mutex));
        broadcast_packet(tourist, &packet, sys_info->tourist_no);
        debug_print(INFO_CHANGE_STATE, tourist, "Changing state to WAIT_SUBMAR");
        tourist->state = WAIT_SUBMAR;
        pthread_mutex_unlock(&(tourist->state_mutex));
    } else {
        debug_print(INFO_WAITING, tourist, "No free submarine found, waiting for one to return");
        wait_for_signal(&(tourist->general_cond));
        debug_print(INFO_OTHER, tourist, "I've noticed that a submarine has returned, waking up");
    }
}

void handler_wait_submar(tourist_t *tourist, system_info_t *sys_info) {
    debug_printf(INFO_OTHER, tourist, "Waiting for access to the %d submarine", tourist->my_submarine);
    wait_for_signal(&(tourist->rec_ack_cond));
    if (can_board(tourist, sys_info)) {
        debug_printf(INFO_CHANGE_STATE, tourist, "Can board %d, changing state to BOARDED", tourist->my_submarine);
        change_state(tourist, BOARDED);
    } else {
        (tourist->try_no)++;
        if (tourist->try_no < sys_info->max_try_no) {
            tourist->list_submar[tourist->my_submarine] = UNAVAILABLE;
            debug_printf(INFO_CHANGE_STATE, tourist, "Can't board %d, trying another one");
            change_state(tourist, CHOOSE_SUBMAR);
        } else {
            debug_printf(INFO_WAITING, tourist,  "Can't fit in %d but I've given up and decided to wait", tourist->my_submarine);
            wait_for_signal(&(tourist->general_cond));
            debug_printf(INFO_CHANGE_STATE, tourist, "The submaring %d has returned, boarding now", tourist->my_submarine);
            change_state(tourist, BOARDED);
        }
    }
}

void handler_boarded(tourist_t *tourist, system_info_t *sys_info) {
    packet_t packet;
    if (is_captain(tourist)) { // TODO: Sometimes he can be that last passenger
        debug_printf(INFO_WAITING, tourist, "I'm a captain of %d, waiting for the submarine to get full", tourist->my_submarine);
        wait_for_signal(&(tourist->full_submar_cond));
        fill_travelling_with_me(tourist, sys_info);
        if (stack_get_size(&(tourist->travelling_with_me)) == 1) {
            debug_printf(INFO_OTHER, tourist, "Submarine %d is full and I'm alone", tourist->id);
        } else {
            debug_printf(INFO_SENDING, tourist, "Submarine %d is full, asking for permission to start the travel", tourist->my_submarine);
            packet.type = ACK_TRAVEL;
            send_packet_to_travelling_with_me(tourist, &packet);
            debug_print(INFO_WAITING, tourist, "Waiting for answers");
            wait_for_signal(&(tourist->ack_travel_cond));
            debug_print(INFO_OTHER, tourist, "Received all of the ACK_TRAVEL");
            if (get_shared_int(&(tourist->is_my_submarine_full))) {
                packet.type = DEPART_SUBMAR;
                debug_print(INFO_SENDING, tourist, "Informing passengers on %d that the submarine is leaving");
            } else {
                packet.type = DEPART_SUBMAR_NOT_FULL;
                debug_print(INFO_SENDING, tourist, "Informing passengers on %d that submarines are leaving not full");
            }
            send_packet_to_travelling_with_me(tourist, &packet);
            debug_print(INFO_WAITING, tourist, "Waiting for answers");
            wait_for_signal(&(tourist->travel_cond));
            debug_print(INFO_CHANGE_STATE, tourist, "Received all of the ACK_TRAVEL, changing state to TRAVEL");
            change_state(tourist, TRAVEL);
        }
    } else {
        wait_for_signal(&(tourist->travel_cond));
        debug_print(INFO_CHANGE_STATE, tourist, "Changing state to TRAVEL");
    }
}

void handler_travel(tourist_t *tourist, system_info_t *sys_info) {
    debug_printf(INFO_OTHER, tourist, "Going on a journey on the submarine %d", tourist->my_submarine);
    if (is_captain(tourist)) {
        int sleep_time = rand() % sys_info->max_travel_time + sys_info->min_travel_time;
        debug_printf(INFO_OTHER, tourist, "Journey on %d will take %d seconds", tourist->my_submarine, sleep_time);
        sleep(sleep_time);
        packet_t packet;
        packet.type = RETURN_SUBMAR;
        packet.submar_id = tourist->my_submarine;
        packet.passenger_no = stack_get_size(&(tourist->travelling_with_me));
        debug_printf(INFO_SENDING, tourist, "Journey on %d has ended, informing passengers", tourist->my_submarine);
        send_packet_to_travelling_with_me(tourist, &packet);
        debug_print(INFO_WAITING, tourist, "Waiting for all of the ACK_TRAVEL responses");
        stack_remove_from_beginning(&(tourist->queues_submar[tourist->my_submarine]), packet.passenger_no);
        wait_for_signal(&(tourist->ack_travel_cond));
        debug_print(INFO_RECEIVING, tourist, "Received all of the ACK_TRAVEL messages, informing other tourists");
        for (int other_id=0; other_id<sys_info->tourist_no; other_id++) {
            if (other_id == tourist->id) continue; // Don't send message to yourself
            for (int co_traveller_id=0; co_traveller_id<stack_get_size(&(tourist->travelling_with_me)); co_traveller_id++) {
                if (other_id == co_traveller_id) continue; // They've already got this message
            }
            send_packet(tourist, &packet, other_id);
        }
    } else {
        debug_print(INFO_WAITING, tourist, "Waiting for the journey to end");
        wait_for_signal(&(tourist->end_travel_cond));
    }
    stack_clear(&(tourist->travelling_with_me));
    debug_print(INFO_CHANGE_STATE, tourist, "Getting on shore");
    change_state(tourist, ON_SHORE);
}

void handler_on_shore(tourist_t *tourist, system_info_t *sys_info) {
    UNUSED(sys_info);
    pthread_mutex_lock(&(tourist->queue_pony_mutex));
    int acks_to_send = stack_get_size(&tourist->queue_pony);
    if (acks_to_send > 0) {
        packet_t packet;
        packet.type = ACK_PONY;
        debug_printf(INFO_SENDING, tourist, "Sending ACK_PONY to %d processes");
        for (int i=0; i<acks_to_send; i++) {
            send_packet(tourist, &packet, stack_pop(&(tourist->queue_pony)));
        }
    }
    pthread_mutex_unlock(&(tourist->queue_pony_mutex));
    debug_print(INFO_CHANGE_STATE, tourist, "Going to sleep");
    change_state(tourist, RESTING);
}
