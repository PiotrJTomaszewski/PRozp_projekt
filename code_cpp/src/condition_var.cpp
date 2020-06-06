#include "condition_var.h"

ConditionVar::ConditionVar() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    signal = NO_SIGNAL;
}

ConditionVar::~ConditionVar() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void ConditionVar::notify(signal_t new_signal) {
    pthread_mutex_lock(&mutex);
    signal |= static_cast<int>(new_signal);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);
}

int ConditionVar::wait_for(int awaited_signals) {
    int encountered_signal;
    pthread_mutex_lock(&mutex);
    // Thread can wait for more than one type of signal, hence this binary and
    while ((signal & awaited_signals) == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    encountered_signal = signal;
    signal = 0x00;
    pthread_mutex_unlock(&mutex);
    return encountered_signal;
}

int ConditionVar::wait_for(signal_t awaited_signal) {
    return wait_for(static_cast<int>(awaited_signal));
}
