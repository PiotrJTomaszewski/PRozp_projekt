#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "tourist.h"

typedef enum DebugMsgTypes {
    INFO_SENDING,
    INFO_RECEIVING,
    ERROR_SENDING,
    ERROR_RECEIVING,
} debug_msg_type_t;

void debug_print(debug_msg_type_t msg_type, int id, state_t state, char *text);

#endif