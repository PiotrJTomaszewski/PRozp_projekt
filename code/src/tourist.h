#ifndef __TOURIST_H__
#define __TOURIST_H__

#include <stdbool.h>
#include <pthread.h>

#include "system_info.h"
#include "stack.h"
#include "shared_var.h"
#include "cond_signal.h"

typedef enum States {
    RESTING,
    WAIT_PONY,
    CHOOSE_SUBMAR,
    WAIT_SUBMAR,
    BOARDED,
    TRAVEL,
    ON_SHORE
} state_t;

typedef enum SubmarState {
    AVAILABLE,
    UNAVAILABLE
} submar_state_t;

extern const char *stateNames[];

typedef struct Tourist {
    int id;
    state_t state;
    shared_int_t lamport_clock;
    int rec_ack_no;
    // TODO: Consider smaller vars
    stack_t queue_pony; // volatile
    stack_t *queues_submar; // A list of stacks
    int my_submarine;
    shared_int_t is_my_submarine_full; // Volatile
    int try_no;
    submar_state_t *list_submar;
    stack_t travelling_with_me;
    pthread_mutex_t state_mutex;
    pthread_mutex_t queue_pony_mutex;
    cond_signal_t general_cond;
    cond_signal_t rec_ack_cond;
    cond_signal_t full_submar_cond;
    cond_signal_t ack_travel_cond;
    cond_signal_t travel_cond;
    cond_signal_t end_travel_cond;
} tourist_t;

bool init_tourist(tourist_t *tourist, system_info_t *info);

bool destroy_tourist(tourist_t *tourist, system_info_t *info);

void change_state(tourist_t *tourist, state_t new_state);

int get_best_submarine(tourist_t *tourist, system_info_t *info);

bool can_board(tourist_t *tourist, system_info_t *info);

bool is_captain(tourist_t *tourist);

void fill_travelling_with_me(tourist_t *tourist, system_info_t *info);

#endif
