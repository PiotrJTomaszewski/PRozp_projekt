#ifndef __TOURIST_H__
#define __TOURIST_H__

#include <stdbool.h>

#include "system_info.h"

enum States {
    RESTING,
    WAIT_PONY,
    CHOOSE_SUBMAR,
    WAIT_SUBMAR,
    BOARDED,
    TRAVEL,
    ON_SHORE
};
typedef enum States state_t;

typedef struct Tourist {
    int id;
    state_t state;
    int lamport_clock;
    int rec_ack_no;
    // TODO: Consider smaller vars
    int *queue_pony;
    int queue_pony_head;
    int **queue_submar;
    int *queue_submar_head;
    int try_no;
    int max_try_no;
    bool *list_submar;

};

bool init_tourist(struct Tourist *tourist, struct SystemInfo *info);

bool finalize_tourist(struct Tourist *tourist);

#endif