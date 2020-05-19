#pragma once

#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

class ConditionVar {
public:
    ConditionVar();
    ~ConditionVar();
    std::unique_lock<std::mutex> mutex_lock();
    // void mutex_unlock();
    // void wait();
    // void wait(std::unique_ptr<std::mutex> cond_lock);
    // void notify();
    std::condition_variable cond_var;

    // inline void notify();
    void notify(std::unique_lock<std::mutex> &mutex);
    void wait(std::unique_lock<std::mutex> &mutex);
    void wait_no_relock(std::unique_lock<std::mutex> &mutex);

private:
    std::mutex cond_mutex;
    std::atomic<bool> was_signal_sent;
    // std::unique_lock<std::mutex> cond_lock;
    // TODO: Add a boolean to prevent spourious wakeups
};
