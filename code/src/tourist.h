#ifndef __TOURIST_H__
#define __TOURIST_H__

#include <stdbool.h>
#include <pthread.h>

#include "system_info.h"

typedef enum States {
    RESTING,
    WAIT_PONY,
    CHOOSE_SUBMAR,
    WAIT_SUBMAR,
    BOARDED,
    TRAVEL,
    ON_SHORE
} state_t;

extern const char *stateNames[];

typedef struct Tourist {
    int id;
    state_t state; // VOLATILE
    int lamport_clock; // VOLATILE
    int rec_ack_no; // VOLATILE
    // TODO: Consider smaller vars
    int *queue_pony; // VOLATILE
    int queue_pony_head; // The next index // VOLATILE
    int **queue_submar; // VOLATILE
    int *queue_submar_head; // The next index for all of the submarines // VOLATILE
    int my_submarine;
    int try_no;
    bool *list_submar;  // False if waiting, True if in travel // VOLATILE
    pthread_mutex_t state_mutex;
    pthread_mutex_t lamport_mutex;
    pthread_cond_t general_cond;
    pthread_mutex_t general_cond_mutex;
    pthread_cond_t rec_ack_cond;
    pthread_mutex_t rec_ack_mutex;
} tourist_t;

bool init_tourist(tourist_t *tourist, system_info_t *info);

bool destroy_tourist(tourist_t *tourist, system_info_t *info);

void change_state(tourist_t *tourist, state_t new_state);

int get_best_submarine(tourist_t *tourist, system_info_t *info);

void wait_for_general_signal(tourist_t *tourist);

void wait_for_rec_ack_signal(tourist_t *tourist);

bool can_board(tourist_t *tourist, system_info_t *info);

#endif