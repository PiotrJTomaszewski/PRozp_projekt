#include "condition_var.h"

ConditionVar::ConditionVar() {

}

ConditionVar::~ConditionVar() {

}

void ConditionVar::mutex_lock() {
    mutex.lock();
}

void ConditionVar::mutex_unlock() {
    mutex.unlock();
}

void ConditionVar::wait() {
    cond_var.wait(mutex);
}

void ConditionVar::notify() {
    cond_var.notify_one();
}
