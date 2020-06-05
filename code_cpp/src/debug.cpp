#include <stdarg.h>
#include <string>

#include "debug.h"
#include "shared_var.h"

static const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

void dumpSubmarineQueues(Tourist &tourist) {
    tourist.submarine_queues->mutex_lock();
    for (int i=0; i<4; i++) {
        printf("Submar %d: [", i);
        for (int j=0; j<tourist.submarine_queues->unsafe_get_size(i); j++) {
            printf("%d, ", tourist.submarine_queues->unsafe_get_tourist_id(i, j));
        }
        printf("]; ");
    }
    tourist.submarine_queues->mutex_unlock();
}

void Debug::dprint( Tourist &tourist, std::string text) {
    dumpSubmarineQueues(tourist);
    int tourist_id = tourist.get_id();
    if (tourist_id + 1 <= 6) {
    printf("\033[0;3%dmId %d - Clock %d - %s - %s\033[0m\n", tourist_id+1, tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    } else {
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    }
}

void Debug::dprintf(Tourist &tourist, std::string format, ...) {
    dumpSubmarineQueues(tourist);
    va_list args;
    const char *format_c = format.c_str();
    va_start(args, format);
    int tourist_id = tourist.get_id();
    if (tourist_id + 1 <= 6) {
        printf("\033[0;3%dmId %d - Clock %d - %s - ", tourist_id+1, tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts("\033[0m");
    } else {
        printf("Id %d - Clock %d - %s - ", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts(""); // Print new line
    }
    va_end(args);
}
