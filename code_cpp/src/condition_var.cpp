#include "condition_var.h"

ConditionVar::ConditionVar() {
}

ConditionVar::~ConditionVar() {

}

void ConditionVar::mutex_lock() {
    cond_mutex.lock();
}

void ConditionVar::mutex_unlock() {
    cond_mutex.unlock();
}

void ConditionVar::wait() {
    std::unique_lock<std::mutex> lock = std::unique_lock<std::mutex>(cond_mutex);
    cond_var.wait(lock);
}

void ConditionVar::notify() {
    cond_var.notify_one();
}
