#include "queues_submarine.h"

QueuesSubmarine::QueuesSubmarine(int submarines_no) {
    // We're creating an array of vectors
    queues = std::unique_ptr<std::vector<int>[]>(new std::vector<int>[submarines_no]);
}

QueuesSubmarine::~QueuesSubmarine() {
}

void QueuesSubmarine::mutex_lock() {
    mutex.lock();
}

void QueuesSubmarine::mutex_unlock() {
    mutex.unlock();
}

void QueuesSubmarine::safe_push_back(int submarine_id, int tourist_id) {
    std::lock_guard<std::mutex> guard(mutex);
    queues[submarine_id].push_back(tourist_id);
}

int QueuesSubmarine::safe_get_tourist_id(int submarine_id, int position) {
    std::lock_guard<std::mutex> guard(mutex);
    return queues[submarine_id][position];
}

int QueuesSubmarine::safe_get_size(int submarine_id) {
    std::lock_guard<std::mutex> guard(mutex);
    return queues[submarine_id].size();
}

void QueuesSubmarine::safe_remove_from_begin(int submarine_id, int number) {
    std::lock_guard<std::mutex> guard(mutex);
    auto begin = queues[submarine_id].begin();
    queues[submarine_id].erase(begin, begin+number);
}

void QueuesSubmarine::safe_remove_tourist_id(int submarine_id, int tourist_id) {
    int position;
    for (position = 0; position < static_cast<int>(queues[submarine_id].size()); position++) {
        if (tourist_id == queues[submarine_id][position]) break;
    }
    queues[submarine_id].erase(queues[submarine_id].begin() + position);
}

void QueuesSubmarine::unsafe_push_back(int submarine_id, int tourist_id) {
    queues[submarine_id].push_back(tourist_id);
}

int QueuesSubmarine::unsafe_get_tourist_id(int submarine_id, int position) {
    return queues[submarine_id][position];
}

int QueuesSubmarine::unsafe_get_size(int submarine_id) {
    return queues[submarine_id].size();
}
