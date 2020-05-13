#pragma once

#include <vector>
#include <mutex>
#include <memory>

class QueuesSubmarine {
public:
    QueuesSubmarine(int submarines_no);
    ~QueuesSubmarine();

    void mutex_lock();
    void mutex_unlock();

    void safe_push_back(int submarine_id, int tourist_id);
    int safe_get_tourist_id(int submarine_id, int position);
    int safe_get_size(int submarine_id);
    void safe_remove_from_begin(int submarine_id, int number);

    void unsafe_push_back(int submarine_id, int tourist_id);
    int unsafe_get_tourist_id(int submarine_id, int position);
    int unsafe_get_size(int submarine_id);

private:
    std::unique_ptr<std::vector<int>[]> queues;
    std::mutex mutex;
};
