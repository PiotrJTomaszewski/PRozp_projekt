#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "debug.h"
#include "tourist.h"

#define NOTIMPLEMENTED(a) ((void)a)
#define BUFFER_SIZE 256

void debug_print(debug_msg_type_t msg_type, tourist_t *tourist, char *text) {
    // TODO: Implement colors or sth like that
    printf("Id %d - Clock %d - %s - %s\n", tourist->id, tourist->lamport_clock, stateNames[tourist->state], text);
}

void debug_printf(debug_msg_type_t msg_type, tourist_t *tourist, char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("Id %d - Clock %d - %s - ", tourist->id, tourist->lamport_clock, stateNames[tourist->state]);
    vprintf(format, args);
    puts(""); // Print new line
    va_end(args);
}