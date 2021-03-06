#pragma once

#include <pthread.h>

class ConditionVar {
public:
    ConditionVar();
    ~ConditionVar();
    enum signal_t {
        NO_SIGNAL = 0x00,
        ENOUGH_ACK_SIGNAL = 0x01,
        ALL_ACK_SIGNAL = 0x02,
        ANY_SUBMARINE_RETURNED_SIGNAL = 0x04,
        MY_SUBMARINE_RETURNED_SIGNAL = 0x08,
        SUBMARINE_FULL_SIGNAL = 0x10,
        DEADLOCK_DETECTED_SIGNAL = 0x20,
        JOURNEY_START_SIGNAL = 0x40,
        JOURNEY_END_SIGNAL = 0x80,
        ANY_SIGNAL = 0xff
    };
    void notify(signal_t signal);
    int wait_for(int awaited_signals);
    int wait_for(signal_t awaited_signal);

private:
    int signal;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

};
