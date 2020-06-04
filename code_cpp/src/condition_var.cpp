#include <memory>
#include <mutex>

#include "condition_var.h"

ConditionVar::ConditionVar() {
    // cond_lock = std::unique_lock<std::mutex>(cond_mutex);
    // cond_lock.unlock();
    is_signal_ready = false;
    sem_init(&semaphore, 0, 0);
}

ConditionVar::~ConditionVar() {
    sem_destroy(&semaphore);
}

// std::unique_lock<std::mutex> ConditionVar::mutex_lock() {
//     return std::unique_lock<std::mutex>(cond_mutex);
// }

// void ConditionVar::mutex_lock() {
//     // cond_mutex.lock();
//     cond_lock.lock();
// }

// void ConditionVar::mutex_unlock() {
//     // cond_mutex.unlock();
//     cond_lock.unlock();
// }

// void ConditionVar::wait() {
//     // std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(cond_mutex);
//     cond_var.wait(cond_lock);
// }

// void ConditionVar::wait(std::unique_ptr<std::mutex> cond_lock) {

// }

// void ConditionVar::notify() {
//     cond_var.notify_one();
// }

// void ConditionVar::notify(std::unique_lock<std::mutex> &mutex) {
//     was_signal_sent = true;
//     cond_var.notify_one();
//     mutex.unlock();

// }

// void ConditionVar::wait(std::unique_lock<std::mutex> &mutex) {
//     while (!was_signal_sent) {
//         cond_var.wait(mutex);
//         // mutex.lock();
//     }
//     was_signal_sent = false;
// }

// void ConditionVar::wait_no_relock(std::unique_lock<std::mutex> &mutex) {
//     was_signal_sent.store(false);
//     while (!was_signal_sent.load()) {
//         cond_var.wait(mutex);
//         if (!was_signal_sent.load()) {
//             mutex.lock();
//         }
//     }
// }


void ConditionVar::notify(ConditionVar::ExtraVarType extra_var) {
    // is_signal_ready = true;
    this->extra_var.store(extra_var);
    sem_post(&semaphore);
}

ConditionVar::ExtraVarType ConditionVar::wait() {
    sem_wait(&semaphore);
    auto var = extra_var.load();
    extra_var.store(NO_VAR);
    return var;
    // is_signal_ready = false;
}

bool ConditionVar::get_is_signal_ready() {
    return is_signal_ready.load();
}

ConditionVar::ExtraVarType ConditionVar::wait_for(ConditionVar::ExtraVarType value) {
    bool should_wait = true;
    ExtraVarType var;
    while (should_wait) {
        sem_wait(&semaphore);
        var = extra_var.load();
        should_wait = !(var == value);
    }
    extra_var.store(NO_VAR);
    return var;
}