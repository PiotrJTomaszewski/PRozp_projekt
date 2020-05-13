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
    void safe_set_element(T value, int position);
    void safe_push_back(T value);

    T unsafe_get_element(int position);
    void unsafe_set_element(T value, int position);
    int unsafe_get_size();
    T unsafe_pop();
    // T& operator [](int position);

private:
    std::vector<T> data;
    std::mutex mutex;
};
