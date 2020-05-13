#pragma once

#include <mutex>

template <class T>
class SharedVar {
public:
    SharedVar();
    SharedVar(T value);
    void safe_set(T value);
    T safe_get();
    void unsafe_set(T value);
    T unsafe_get();
    T* get_pointer();
    void mutex_lock();
    void mutex_unlock();
private:
    std::mutex mutex;
};
