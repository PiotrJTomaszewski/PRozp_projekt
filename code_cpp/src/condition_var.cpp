#include <memory>
#include <mutex>

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
    signal_t old_signal;
    pthread_mutex_lock(&mutex);
    old_signal = signal;
    signal = new_signal;
    pthread_mutex_unlock(&mutex);
    // if (old_signal == NO_SIGNAL) {
        pthread_cond_signal(&cond);
    // }
}

ConditionVar::signal_t ConditionVar::wait_for(signal_t awaited_signals) {
    signal_t encountered_signal;
    pthread_mutex_lock(&mutex);
    // Thread can wait for more than one type of signal, hence this binary and
    while ((signal & awaited_signals) == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    encountered_signal = signal;
    signal = NO_SIGNAL;
    pthread_mutex_unlock(&mutex);
    return encountered_signal;
}

ConditionVar::signal_t ConditionVar::wait_for(int awaited_signals) {
    return wait_for(static_cast<signal_t>(awaited_signals));
}
