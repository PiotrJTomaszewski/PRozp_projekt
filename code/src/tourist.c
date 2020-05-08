#include <stdlib.h>
#include <stdbool.h>

#include "tourist.h"
#include "system_info.h"
#include "stack.h"
#include "shared_var.h"
#include "debug.h"
#include "cond_signal.h"

const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

bool init_tourist(tourist_t *tourist, system_info_t *info) {
    bool error_occurred = false;
    
    error_occurred |= init_shared_int(&(tourist->lamport_clock));
    tourist->lamport_clock.value = 0;

    error_occurred |= init_shared_int(&(tourist->is_my_submarine_full));
    tourist->is_my_submarine_full.value = 0;

    tourist->state = RESTING;
    error_occurred |= pthread_mutex_init(&(tourist->state_mutex), NULL);
    tourist->rec_ack_no = 1; // Tourist always have his own approval

    error_occurred |= pthread_mutex_init(&(tourist->queue_pony_mutex), NULL);

    error_occurred |= init_cond_signal(&(tourist->general_cond));
    error_occurred |= init_cond_signal(&(tourist->rec_ack_cond));
    error_occurred |= init_cond_signal(&(tourist->full_submar_cond));
    error_occurred |= init_cond_signal(&(tourist->ack_travel_cond));
    error_occurred |= init_cond_signal(&(tourist->travel_cond));
    error_occurred |= init_cond_signal(&(tourist->end_travel_cond));

    error_occurred |= stack_init(&(tourist->queue_pony), info->tourist_no);

    tourist->my_submarine = -1;
    tourist->queues_submar = malloc(info->submar_no * sizeof(stack_t));
    error_occurred |= (tourist->queues_submar == NULL);
    for (int i=0; i<info->submar_no; i++) {
        error_occurred |= stack_init(&(tourist->queues_submar[i]), info->tourist_no);
    }

    error_occurred |= stack_init(&(tourist->travelling_with_me), info->tourist_no);

    tourist->list_submar = calloc(info->submar_no, sizeof(submar_state_t));
    error_occurred |= (tourist->list_submar == NULL);

    return error_occurred;
}


bool destroy_tourist(tourist_t *tourist, system_info_t *info) {
    stack_destroy(&(tourist->queue_pony));
    for (int i=0; i<info->submar_no; i++) {
        stack_destroy(&(tourist->queues_submar[i]));
    }
    free(tourist->queues_submar);
    destroy_shared_int(&(tourist->lamport_clock));
    destroy_shared_int(&(tourist->is_my_submarine_full));
    pthread_mutex_destroy(&(tourist->state_mutex)); // TODO: Check for errors
    pthread_mutex_destroy(&(tourist->queue_pony_mutex));

    destroy_cond_signal(&(tourist->general_cond));
    destroy_cond_signal(&(tourist->rec_ack_cond));
    destroy_cond_signal(&(tourist->full_submar_cond));
    destroy_cond_signal(&(tourist->ack_travel_cond));
    destroy_cond_signal(&(tourist->travel_cond));
    destroy_cond_signal(&(tourist->end_travel_cond));
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
        if (tourist->list_submar[submar_id] == UNAVAILABLE)
            continue;
        submar_cap = info->dict_submar_capacity[submar_id];
        if ( stack_get_size(&(tourist->queues_submar[submar_id])) > 0) {
            left_submar_cap = info->dict_submar_capacity[submar_id];
            for (int i=0; i<stack_get_size(&(tourist->queues_submar[submar_id])); i++) {
                left_submar_cap -= info->dict_tourist_sizes[stack_get_value_at(&(tourist->queues_submar[submar_id]), i)];
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

bool can_board(tourist_t *tourist, system_info_t *info) {
    int submar_id = tourist->my_submarine;
    int space_left = info->dict_submar_capacity[submar_id];
    for (int i=0; i<stack_get_size(&(tourist->queues_submar[submar_id])); i++) {
        space_left -= info->dict_tourist_sizes[stack_get_value_at(&(tourist->queues_submar[submar_id]), i)];
    }
    return (space_left >= info->dict_tourist_sizes[tourist->id]);
}

bool is_captain(tourist_t *tourist) {
    return (tourist->id == stack_get_value_at(&(tourist->queues_submar[tourist->my_submarine]), 0));
}

void fill_travelling_with_me(tourist_t *tourist, system_info_t *info) {
    stack_clear(&(tourist->travelling_with_me));
    int submar_id = tourist->my_submarine;
    int submar_cap_left = info->dict_submar_capacity[submar_id];
    int other_tourist_id, other_tourist_size;
    for (int i=0; i<stack_get_size(&(tourist->queues_submar[submar_id])); i++) {
        other_tourist_id = stack_get_value_at(&(tourist->queues_submar[submar_id]), i);
        other_tourist_size = info->dict_tourist_sizes[other_tourist_id];
        submar_cap_left -= other_tourist_size;
        if (submar_cap_left >= 0) {
            stack_push(&(tourist->travelling_with_me), other_tourist_id);
        } else {
            break; // The submarine is full
        }
    }
}
