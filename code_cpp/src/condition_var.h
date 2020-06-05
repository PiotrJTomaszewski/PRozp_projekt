#pragma once

#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

#include <pthread.h>
#include <semaphore.h>

class ConditionVar {
public:
    ConditionVar();
    ~ConditionVar();
    // std::unique_lock<std::mutex> mutex_lock();
    // void mutex_unlock();
    // void wait();
    // void wait(std::unique_ptr<std::mutex> cond_lock);
    // void notify();

    // inline void notify();
    // void notify(std::unique_lock<std::mutex> &mutex);
    // void wait(std::unique_lock<std::mutex> &mutex);
    // void wait_no_relock(std::unique_lock<std::mutex> &mutex);

    enum ExtraVarType {
        NO_VAR,
        DEADLOCK_DETECTED,
        SUBMARINE_FULL,
        MY_SUBMARINE,
        ANY_SUBMARINE,
        ENOUGH_ACK,
        ALL_ACK,
        SUBMARINE_RETURN,
        IN_CHOOSE_SUBMAR,
        IN_WAIT_SUBMAR,
        IN_TRAVEL
    };
    void lock_mutex();
    void unlock_mutex();
    void notify(ExtraVarType extra);
    ExtraVarType wait();
    ExtraVarType wait_for(ExtraVarType value);
    bool get_is_signal_ready();


private:
    // std::condition_variable cond_var;
    // std::condition_variable cond_var_producer;
    // std::condition_variable cond_var_consumer;
    // std::mutex cond_mutex;
    std::atomic<bool> is_signal_ready;
    sem_t semaphore;
    // std::unique_lock<std::mutex> cond_lock;
    // TODO: Add a boolean to prevent spourious wakeups
    std::atomic<ExtraVarType> extra_var;
};
