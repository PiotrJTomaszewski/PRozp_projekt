#include <memory>
#include <mutex>

#include "condition_var.h"

ConditionVar::ConditionVar() {
    // cond_lock = std::unique_lock<std::mutex>(cond_mutex);
    // cond_lock.unlock();
}

ConditionVar::~ConditionVar() {

}

std::unique_lock<std::mutex> ConditionVar::mutex_lock() {
    return std::unique_lock<std::mutex>(cond_mutex);
}

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

void ConditionVar::notify(std::unique_lock<std::mutex> &mutex) {
    was_signal_sent.store(true);
    cond_var.notify_one();
    mutex.unlock();

}

void ConditionVar::wait(std::unique_lock<std::mutex> &mutex) {
    was_signal_sent.store(false);
    while (!was_signal_sent.load()) {
        cond_var.wait(mutex);
        mutex.lock();
    }
}

void ConditionVar::wait_no_relock(std::unique_lock<std::mutex> &mutex) {
    was_signal_sent.store(false);
    while (!was_signal_sent.load()) {
        cond_var.wait(mutex);
        if (!was_signal_sent.load()) {
            mutex.lock();
        }
    }
}
