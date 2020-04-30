#include <stdio.h>

#include "debug.h"
#include "tourist.h"

void debug_print(debug_msg_type_t msg_type, int id, state_t state, char *text) {  
    // TODO: Implement colors or sth like that
    char *state_name = stateNames[state];
    printf("[%d] - %s - %s\n", id, state_name, text);
}