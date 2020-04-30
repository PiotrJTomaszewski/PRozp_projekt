#include <stdlib.h>
#include <stdbool.h>

#include "tourist.h"
#include "system_info.h"

bool init_tourist(tourist_t *tourist, system_info_t *info) {
    tourist->lamport_clock = 0;
    tourist->state = RESTING;
    tourist->queue_pony = calloc(info->tourist_no, sizeof tourist->queue_pony);
    if (tourist->queue_pony == NULL)
        return false;
    tourist->queue_pony_head = 0;

    tourist->queue_submar = malloc(info->submar_no * sizeof tourist->queue_submar);
    if (tourist->queue_submar == NULL)
        return false;
    for (int i=0; i<info->submar_no; i++) {
        tourist->queue_submar[i] = calloc(info->tourist_no, sizeof tourist->id);
        if (tourist->queue_submar[i] == NULL)
            return false;
    }
    tourist->queue_submar_head = calloc(info->submar_no, sizeof tourist->queue_submar_head);
    if (tourist->queue_submar_head == NULL)
        return false;
    return true;
}


bool finalize_tourist(tourist_t *tourist, system_info_t *info) {
    free(tourist->queue_pony);
    free(tourist->queue_submar_head);
    for (int i=0; i<info->submar_no; i++) {
        free(tourist->queue_submar[i]);
    }
    free(tourist->queue_submar);
}