#include "tourist.h"

class Debug {
public:
    enum debug_msg_type_t {
        INFO_SENDING,
        INFO_RECEIVING,
        INFO_CHANGE_STATE,
        INFO_WAITING,
        INFO_OTHER,
        ERROR_SENDING,
        ERROR_RECEIVING,
        ERROR_OTHER
    };
    static void dprint(debug_msg_type_t msg_type, Tourist &tourist, char *text);
    static void dprintf(debug_msg_type_t, Tourist &tourist, char *format, ...);
};
