#include <stdarg.h>
#include <string>

#include "debug.h"
#include "shared_var.h"

static const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

void Debug::dprint( Tourist &tourist, std::string text) {
    // TODO: Implement colors or sth like that
    int tourist_id = tourist.get_id();
    if (tourist_id + 1 <= 6) {
    printf("\033[0;3%dmId %d - Clock %d - %s - %s\033[0m\n", tourist_id+1, tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    } else {
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    }
}

void Debug::dprintf(Tourist &tourist, std::string format, ...) {
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
