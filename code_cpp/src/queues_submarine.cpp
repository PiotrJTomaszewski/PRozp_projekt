#include <algorithm>
#include "queues_submarine.h"

QueuesSubmarine::QueuesSubmarine(int submarines_no) {
    // We're creating an array of vectors
    queues = std::unique_ptr<std::vector<Element>[]>(new std::vector<Element>[submarines_no]);
}

QueuesSubmarine::~QueuesSubmarine() {
}

void QueuesSubmarine::mutex_lock() {
    mutex.lock();
}

void QueuesSubmarine::mutex_unlock() {
    mutex.unlock();
}

bool QueuesSubmarine::is_lower_priority(Element element1, Element element2) {
    if (element1.tourist_clock > element2.tourist_clock) {
        return true;
    }
    if (element1.tourist_clock == element2.tourist_clock) {
        if (element1.tourist_id > element2.tourist_id) {
            return true;
        }
    }
    return false;
}

void QueuesSubmarine::safe_add(int submarine_id, int tourist_id, int tourist_clock) {
    std::lock_guard<std::mutex> guard(mutex);
    unsafe_add(submarine_id, tourist_id, tourist_clock);
}

int QueuesSubmarine::safe_get_tourist_id(int submarine_id, int position) {
    std::lock_guard<std::mutex> guard(mutex);
    return queues[submarine_id][position].tourist_id;
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
        if (tourist_id == queues[submarine_id][position].tourist_id) break;
    }
    queues[submarine_id].erase(queues[submarine_id].begin() + position);
}

void QueuesSubmarine::unsafe_add(int submarine_id, int tourist_id, int tourist_clock) {
    Element new_tourist_element = {tourist_id, tourist_clock};
    auto position = std::find_if(queues[submarine_id].begin(), queues[submarine_id].end(), 
        [new_tourist_element](QueuesSubmarine::Element x){return QueuesSubmarine::is_lower_priority(x, new_tourist_element);}
    );
    queues[submarine_id].insert(position, new_tourist_element);
}

int QueuesSubmarine::unsafe_get_tourist_id(int submarine_id, int position) {
    return queues[submarine_id][position].tourist_id;
}

int QueuesSubmarine::unsafe_get_size(int submarine_id) {
    return queues[submarine_id].size();
}
