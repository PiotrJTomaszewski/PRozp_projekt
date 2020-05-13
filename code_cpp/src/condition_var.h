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
    std::unique_lock<std::mutex> mutex;
    std::condition_variable cond_var;
};
