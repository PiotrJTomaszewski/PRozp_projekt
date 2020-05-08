#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "tourist.h"

typedef enum DebugMsgTypes {
    INFO_SENDING,
    INFO_RECEIVING,
    INFO_CHANGE_STATE,
    INFO_WAITING,
    INFO_OTHER,
    ERROR_SENDING,
    ERROR_RECEIVING,
    ERROR_OTHER
} debug_msg_type_t;

void debug_print(debug_msg_type_t msg_type, tourist_t *tourist, char *text);

void debug_printf(debug_msg_type_t msg_type, tourist_t *tourist, char *format, ...);

#endif