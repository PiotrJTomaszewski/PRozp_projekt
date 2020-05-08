#include <pthread.h>
#include <stdbool.h>

#include "cond_signal.h"

bool init_cond_signal(cond_signal_t *cond_signal) {
    bool error_occurred = false;
    error_occurred |= pthread_cond_init(&(cond_signal->cond), NULL);
    error_occurred |= pthread_mutex_init(&(cond_signal->mutex), NULL);
    return error_occurred;
}

void destroy_cond_signal(cond_signal_t *cond_signal) {
    // TODO: Check for errors
        pthread_cond_destroy(&(cond_signal->cond));
    pthread_mutex_destroy(&(cond_signal->mutex));
}

void wait_for_signal(cond_signal_t *cond_signal) {
    pthread_mutex_t *mutex = &(cond_signal->mutex);
    pthread_mutex_lock(mutex);
    pthread_cond_wait(&(cond_signal->cond), mutex);
    pthread_mutex_unlock(mutex);
}

void set_signal(cond_signal_t *cond_signal) {
    pthread_mutex_lock(&(cond_signal->mutex));
    pthread_cond_signal(&(cond_signal->cond));
    pthread_mutex_unlock(&(cond_signal->mutex));
}