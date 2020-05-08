#ifndef __COND_SIGNAL_H__
#define __COND_SIGNAL_H__

#include <pthread.h>
#include <stdbool.h>

typedef struct CondSignal {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} cond_signal_t;

bool init_cond_signal(cond_signal_t *cond_signal);

void destroy_cond_signal(cond_signal_t *cond_signal);

void wait_for_signal(cond_signal_t *cond_signal);

void set_signal(cond_signal_t *cond_signal);

#endif
