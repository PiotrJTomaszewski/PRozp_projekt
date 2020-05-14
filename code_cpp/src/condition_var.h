#pragma once

#include <mutex>
#include <condition_variable>

class ConditionVar {
public:
    ConditionVar();
    ~ConditionVar();
    void mutex_lock();
    void mutex_unlock();
    void wait();
    void notify();
private:
    std::mutex cond_mutex;
    std::unique_lock<std::mutex> cond_lock;
    std::condition_variable cond_var;
    // TODO: Add a boolean to prevent spourious wakeups
};
