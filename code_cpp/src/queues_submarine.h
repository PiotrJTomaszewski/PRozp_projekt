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

    void safe_add(int submarine_id, int tourist_id, int tourist_clock);
    int safe_get_tourist_id(int submarine_id, int position);
    int safe_get_size(int submarine_id);
    void safe_remove_from_begin(int submarine_id, int number);
    void safe_remove_tourist_id(int submarine_id, int tourist_id);

    void unsafe_add(int submarine_id, int tourist_id, int tourist_clock);
    int unsafe_get_tourist_id(int submarine_id, int position);
    int unsafe_get_size(int submarine_id);

    struct Element {
        int tourist_id;
        int tourist_clock;
    };
    static bool is_lower_priority(Element element1, Element element2);

private:
    void sort();

    std::unique_ptr<std::vector<Element>[]> queues;
    std::mutex mutex;
    // bool is_lower_priority_than_new_tourist(Element to_test);
    // Element new_tourist_element;
};
