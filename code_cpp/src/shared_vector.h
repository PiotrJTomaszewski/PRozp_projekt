#pragma once

#include <mutex>
#include <vector>

template <class T>
class SharedVector {
public:
    SharedVector();
    SharedVector(const std::vector<T> &init_data);
    SharedVector(T init_data[], int size);
    ~SharedVector();

    void mutex_lock();
    void mutex_unlock();
    T safe_get_element(int position);
    int safe_get_size();
    void safe_set_element(T value, int position);
    void safe_push_back(T value);

    T unsafe_get_element(int position);
    void unsafe_set_element(T value, int position);
    int unsafe_get_size();
    void unsafe_push_back(T value);
    T unsafe_pop();
    void unsafe_clear();

private:
    std::vector<T> data;
    std::mutex mutex;
};

template <class T>
SharedVector<T>::SharedVector() {

}

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

template <class T>
T SharedVector<T>::safe_get_element(int position) {
    std::lock_guard<std::mutex> guard(mutex);
    return data[position];
}

template <class T>
int SharedVector<T>::safe_get_size() {
    std::lock_guard<std::mutex> guard(mutex);
    return data.size();
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
void SharedVector<T>::unsafe_push_back(T value) {
    data.push_back(value);
}

template <class T>
T SharedVector<T>::unsafe_pop() {
    T last_val = data.back();
    data.pop_back();
    return last_val;
}

template <class T>
void SharedVector<T>::unsafe_clear() {
    data.clear();
}
