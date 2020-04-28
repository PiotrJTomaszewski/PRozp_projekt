#ifndef __MESSAGES_H__
#define __MESSAGES_H__

enum Messages {
    REQ_PONY,
    ACK_PONY,
    REQ_SUBMAR, // {submar_id}
    ACK_SUBMAR,
    FULL_SUBMAR_RETREAT, // {submar_id}
    FULL_SUBMAR_STATE, // {submar_id}
    RETURN_SUBMAR, // {submar_id, tourist_no}
    TRAVEL_READY,
    ACK_TRAVEL,
    DEPART_SUBMAR,
    DEPART_SUBMAR_NOT_FULL
};
typedef Messages msg_t;




#endif