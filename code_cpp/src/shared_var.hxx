#include <mutex>

#include "shared_var.h"

template <class T>
SharedVar<T>::SharedVar(T value) {
    this.value = value;
}

template <class T>
void SharedVar<T>::safe_set(T value) {
    std::lock_guard<std::mutex> guard(mutex);
    this.value = value;
}

template <class T>
T SharedVar<T>::safe_get() {
    std::lock_guard<std::mutex> guard(mutex);
    return value;
}

template <class T>
void SharedVar<T>::unsafe_set(T value) {
    this.value = value;
}

template <class T>
T SharedVar<T>::unsafe_get() {
    return value;
}

template <class T>
T* SharedVar<T>::get_pointer() {
    return &value;
}

template<class T>
void SharedVar<T>::mutex_lock() {
    mutex.lock();
}

template<class T>
void SharedVar<T>::mutex_unlock() {
    mutex.unlock();
}
