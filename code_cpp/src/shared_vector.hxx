#include "shared_vector.h"

template <class T>
SharedVector<T>::SharedVector(const std::vector<T> &init_data) {
    // Copy data
    data = init_data;
}

template <class T>
SharedVector<T>::SharedVector(T init_data[], int size) {
    data.reserve(size);
    for (int i = 0; i < size; i++) {
        data.push_back(init_data[i]);
    }
}

template <class T>
SharedVector<T>::~SharedVector() {

}

template <class T>
void SharedVector<T>::mutex_lock() {
    mutex.lock();
}

template <class T>
void SharedVector<T>::mutex_unlock() {
    mutex.unlock();
}

//TODO: This class needs testing
template <class T>
T SharedVector<T>::safe_get_element(int position) {
    std::lock_guard<std::mutex> guard(mutex);
    return data[position];
}

template <class T>
void SharedVector<T>::safe_set_element(T value, int position) {
    std::lock_guard<std::mutex> guard(mutex);
    data[position] = value;
}

template <class T>
void SharedVector<T>::safe_push_back(T value) {
    std::lock_guard<std::mutex> guard(mutex);
    data.push_back(value);
}

template <class T>
T  SharedVector<T>::unsafe_get_element(int position) {
    return data[position];
}

template <class T>
void  SharedVector<T>::unsafe_set_element(T value, int position) {
    data[position] = value;
}

template <class T>
int SharedVector<T>::unsafe_get_size() {
    return data.size();
}

template <class T>
T SharedVector<T>::unsafe_pop() {
    return data.pop_back();
}


// template <class T>
// T& SharedVector<T>::operator [](int position) {
//     std::lock_guard<std::unique_lock> lock(mutex);
//     return &data[position];
// }
