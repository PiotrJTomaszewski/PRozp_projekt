#include <stdlib.h>
#include <stdbool.h>

#include "tourist.h"
#include "system_info.h"

const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

bool init_tourist(tourist_t *tourist, system_info_t *info) {
    bool error_occurred = false;
    tourist->lamport_clock = 0;
    error_occurred |= pthread_mutex_init(&(tourist->lamport_mutex), NULL);
    tourist->state = RESTING;
    error_occurred |= pthread_mutex_init((&tourist->state_mutex), NULL);
    tourist->rec_ack_no = 1; // Tourist always have his own approval
    error_occurred |= pthread_cond_init((&tourist->general_cond), NULL);
    error_occurred |= pthread_mutex_init((&tourist->general_cond_mutex), NULL);

    error_occurred |= pthread_cond_init((&tourist->rec_ack_cond), NULL);
    error_occurred |= pthread_mutex_init((&tourist->rec_ack_mutex), NULL);

    tourist->queue_pony = calloc(info->tourist_no, sizeof tourist->queue_pony);
    error_occurred |= (tourist->queue_pony == NULL);
    tourist->queue_pony_head = 0;

    tourist->my_submarine = -1;
    tourist->queue_submar = malloc(info->submar_no * sizeof tourist->queue_submar);
    error_occurred |= (tourist->queue_submar == NULL);
    for (int i=0; i<info->submar_no; i++) {
        tourist->queue_submar[i] = calloc(info->tourist_no, sizeof tourist->id);
        error_occurred |= (tourist->queue_submar[i] == NULL);
    }
    tourist->queue_submar_head = calloc(info->submar_no, sizeof tourist->queue_submar_head);
    error_occurred |= (tourist->queue_submar_head == NULL);

    tourist->list_submar = calloc(info->submar_no, sizeof(bool));
    error_occurred |= (tourist->list_submar == NULL);
    return error_occurred;
}


bool destroy_tourist(tourist_t *tourist, system_info_t *info) {
    free(tourist->queue_pony);
    free(tourist->queue_submar_head);
    for (int i=0; i<info->submar_no; i++) {
        free(tourist->queue_submar[i]);
    }
    free(tourist->queue_submar);
    pthread_mutex_destroy(&(tourist->lamport_mutex)); // TODO: Check for errors
    pthread_mutex_destroy(&(tourist->state_mutex));
    pthread_cond_destroy(&(tourist->general_cond));
    pthread_mutex_destroy(&(tourist->general_cond_mutex));
    return false;
}

void change_state(tourist_t *tourist, state_t new_state) {
    pthread_mutex_lock(&(tourist->state_mutex));
    tourist->state = new_state;
    pthread_mutex_unlock(&(tourist->state_mutex));
}

int get_best_submarine(tourist_t *tourist, system_info_t *info) {
    int submar_cap;
    int left_submar_cap;
    int empty_submar_id = -1;
    int best_submar_id = -1;
    double current_submar_cap_taken;
    double best_submar_cap_taken = 1.f;
    /* Going in reverse because we want to get the submarine with the lowest id
    and this way we don't have to make extra checks on empty boats*/
    for (int submar_id=info->submar_no-1; submar_id>=0; submar_id--) {
        if (tourist->list_submar[submar_id])
            continue;
        submar_cap = info->dict_submar_capacity[submar_id];
        if (tourist->queue_submar_head[submar_id] > 0) {
            left_submar_cap = info->dict_submar_capacity[submar_id];
            for (int i=0; i<tourist->queue_submar_head[submar_id]; i++) {
                int tourist_id = tourist->queue_submar[submar_id][i];
                left_submar_cap -= info->dict_tourist_sizes[tourist_id];
            }
            if (left_submar_cap >= 0) {
                current_submar_cap_taken = (double)(left_submar_cap) / submar_cap;
                if (current_submar_cap_taken < best_submar_cap_taken) {
                    best_submar_cap_taken = current_submar_cap_taken;
                    best_submar_id = submar_id;
                }
            }
        } else { // Empty boat
            empty_submar_id = submar_id;
        }
    }
    if (best_submar_id == -1) best_submar_id = empty_submar_id;
    return best_submar_id;
}

void wait_for_general_signal(tourist_t *tourist) {
    pthread_mutex_lock(&(tourist->general_cond_mutex));
    pthread_cond_wait(&(tourist->general_cond), &(tourist->general_cond_mutex));
    pthread_mutex_unlock(&(tourist->general_cond_mutex));
}

void wait_for_rec_ack_signal(tourist_t *tourist) {
    pthread_mutex_lock(&(tourist->rec_ack_mutex));
    pthread_cond_wait(&(tourist->rec_ack_cond), &(tourist->rec_ack_mutex));
    pthread_mutex_unlock(&(tourist->rec_ack_mutex));
}

bool can_board(tourist_t *tourist, system_info_t *info) {
    int submar_id = tourist->my_submarine;
    int space_left = info->dict_submar_capacity[submar_id];
    int other_tourist_id;
    for (int i=0; i<tourist->queue_submar_head[submar_id]; i++) {
        other_tourist_id = tourist->queue_submar[submar_id][i];
        space_left -= info->dict_tourist_sizes[other_tourist_id];
    }
    return (space_left >= info->dict_tourist_sizes[tourist->id]);
}