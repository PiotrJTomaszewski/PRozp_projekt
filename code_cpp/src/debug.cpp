#include <stdarg.h>
#include <string>

#include "debug.h"
#include "shared_var.h"

static const char *stateNames[] = {
    "RESTING", "WAIT_PONY", "CHOOSE_SUBMAR", 
    "WAIT_SUBMAR", "BOARDED", "TRAVEL", "ON_SHORE" 
};

#ifdef DEBUG_SHOW_SUBMAR_QUEUE
void printSubmarineQueues(Tourist &tourist) {
    tourist.submarine_queues->mutex_lock();
    for (int i=0; i<6; i++) {
        printf("Submar %d: [", i);
        for (int j=0; j<tourist.submarine_queues->unsafe_get_size(i); j++) {
            printf("%d, ", tourist.submarine_queues->unsafe_get_tourist_id(i, j));
        }
        printf("]; ");
    }
    tourist.submarine_queues->mutex_unlock();
}
#endif //DEBUG_SHOW_SUBMAR_QUEUE

#ifdef DEBUG_USE_COLOR
const int colors_count = 13;
const char* colors[] = {"0;31", "0;32", "0;33", "0;35", "0;36", "0;37", "1;30", "1;31", "1;32", "1;33", "1;34", "1;35", "1;36"};
#endif //DEBUG_USE_COLOR

void Debug::dprint( Tourist &tourist, std::string text) {
    #ifdef DEBUG_SHOW_SUBMAR_QUEUE
    printSubmarineQueues(tourist);
    #endif //DEBUG_SHOW_SUBMAR_QUEUE
    int tourist_id = tourist.get_id();
    #ifdef DEBUG_USE_COLOR
    if (tourist_id < colors_count) {
    printf("\033[%smId %d - Clock %d - %s - %s\033[0m\n", colors[tourist_id], tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    } else {
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    }
    #else //DEBUG_USE_COLOR
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()], text.c_str());
    #endif //DEBUG_USE_COLOR
}

void Debug::dprintf(Tourist &tourist, std::string format, ...) {
    #ifdef DEBUG_SHOW_SUBMAR_QUEUE
    printSubmarineQueues(tourist);
    #endif //DEBUG_SHOW_SUBMAR_QUEUE
    va_list args;
    const char *format_c = format.c_str();
    va_start(args, format);
    int tourist_id = tourist.get_id();
    #ifdef DEBUG_USE_COLOR
    if (tourist_id < colors_count) {
        printf("\033[%smId %d - Clock %d - %s - ", colors[tourist_id], tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts("\033[0m");
    } else {
        printf("Id %d - Clock %d - %s - ", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts(""); // Print new line
    }
    #else //DEBUG_USE_COLOR
    printf("Id %d - Clock %d - %s - ", tourist_id, tourist.lamport_clock.load(), stateNames[tourist.state.unsafe_get()]);
    vprintf(format_c, args);
    puts(""); // Print new line
    #endif //DEBUG_USE_COLOR
    va_end(args);
}

void Debug::dprintp(Tourist &tourist, Packet &packet, std::string text) {
    #ifdef DEBUG_SHOW_SUBMAR_QUEUE
    printSubmarineQueues(tourist);
    #endif //DEBUG_SHOW_SUBMAR_QUEUE
    int tourist_id = tourist.get_id();
    #ifdef DEBUG_USE_COLOR
    if (tourist_id < colors_count) {
    printf("\033[%smId %d - Clock %d - %s - %s\033[0m\n", colors[tourist_id], tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()], text.c_str());
    } else {
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()], text.c_str());
    }
    #else //DEBUG_USE_COLOR
    printf("Id %d - Clock %d - %s - %s\n", tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()], text.c_str());
    #endif //DEBUG_USE_COLOR
}

void Debug::dprintfp(Tourist &tourist, Packet &packet, std::string format, ...) {
    #ifdef DEBUG_SHOW_SUBMAR_QUEUE
    printSubmarineQueues(tourist);
    #endif //DEBUG_SHOW_SUBMAR_QUEUE
    va_list args;
    const char *format_c = format.c_str();
    va_start(args, format);
    int tourist_id = tourist.get_id();
    #ifdef DEBUG_USE_COLOR
    if (tourist_id < colors_count) {
        printf("\033[%smId %d - Clock %d - %s - ", colors[tourist_id], tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts("\033[0m");
    } else {
        printf("Id %d - Clock %d - %s - ", tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()]);
        vprintf(format_c, args);
        puts(""); // Print new line
    }
    #else //DEBUG_USE_COLOR
    printf("Id %d - Clock %d - %s - ", tourist_id, packet.get_timestamp(), stateNames[tourist.state.unsafe_get()]);
    vprintf(format_c, args);
    puts(""); // Print new line
    #endif //DEBUG_USE_COLOR
    va_end(args);
}