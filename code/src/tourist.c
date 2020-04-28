#include "tourist.h"

#include <stdlib.h>

bool init_tourist(struct Tourist *tourist, struct SystemInfo *info) {
    tourist->lamport_clock = 0;
    tourist->state = RESTING;
    tourist->queue_pony = calloc(info->tourist_no, sizeof tourist->queue_pony);
    if (tourist->queue_pony)
        return true;
}


bool finalize_tourist(struct Tourist *tourist) {
    free(tourist->queue_pony);
}