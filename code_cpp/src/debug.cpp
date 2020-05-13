#include <stdarg.h>

#include "debug.h"
#include "shared_var.h"

static const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

void Debug::dprint(debug_msg_type_t msg_type, Tourist &tourist, char *text) {
    // TODO: Implement colors or sth like that
    printf("Id %d - Clock %d - %s - %s\n", tourist.get_id(), tourist.lamport_clock, stateNames[tourist.state.unsafe_get()], text);
}

void Debug::dprintf(debug_msg_type_t, Tourist &tourist, char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("Id %d - Clock %d - %s - ", tourist.get_id, tourist.lamport_clock, stateNames[tourist.state.unsafe_get()]);
    vprintf(format, args);
    puts(""); // Print new line
    va_end(args);
}
