#include <list>
#include <algorithm>
#include "tourist.h"

Tourist::Tourist(int id, int submarine_no) {
    this->id = id;
    submarine_queues = std::unique_ptr<QueuesSubmarine>(new QueuesSubmarine(submarine_no));
    received_ack_no = 1;
    is_ack_travel_queued = false;
    lamport_clock = 0;
    state.unsafe_set(RESTING);
    is_my_submarine_full = false;
    for (int i=0; i<submarine_no; i++) {
        available_submarine_list.unsafe_push_back(true);
    }
}

Tourist::~Tourist() {

}

int Tourist::get_id() {
    return id;
}

int Tourist::get_best_submarine_id(SystemInfo &sys_info) {
    int submarine_capacity;
    int submarine_capacity_left;
    int empty_submarine_id = -1;
    int best_submarine_id = -1;
    int unavailable_submarine_id = 0;
    int submarine_queue_size;
    double current_submarine_capacity_taken;
    double best_submarine_capacity_taken = 1.f;
    available_submarine_list.mutex_lock();
    submarine_queues->mutex_lock();
    /* Going in reverse because we want to get the submarine with the lowest id
    and this way we don't have to make extra checks on empty boats*/
    for (int submarine_id=sys_info.get_submarine_no()-1; submarine_id>=0; submarine_id--) {
        if (!available_submarine_list.unsafe_get_element(submarine_id)) {
            unavailable_submarine_id = submarine_id;
        }
        submarine_capacity = sys_info.get_submarine_capacity(submarine_id);

        submarine_queue_size = submarine_queues->unsafe_get_size(submarine_id);
        if (submarine_queue_size > 0) {
            submarine_capacity_left = sys_info.get_submarine_capacity(submarine_id);
            for (int queue_position=0; queue_position<submarine_queue_size; queue_position++) {
                submarine_capacity_left -= sys_info.get_tourist_size(submarine_queues->unsafe_get_tourist_id(submarine_id, queue_position));
            }
            if (submarine_capacity_left >= 0) {
                current_submarine_capacity_taken = static_cast<double>(submarine_capacity_left) / submarine_capacity;
                if (current_submarine_capacity_taken < best_submarine_capacity_taken) {
                    best_submarine_capacity_taken = current_submarine_capacity_taken;
                    best_submarine_id = submarine_id;
                }
            }
        } else { // Empty submarine
            empty_submarine_id = submarine_id;
        }
    }
    submarine_queues->mutex_unlock();
    available_submarine_list.mutex_unlock();
    if (best_submarine_id == -1) best_submarine_id = empty_submarine_id;
    if (best_submarine_id == -1) best_submarine_id = unavailable_submarine_id;
    return best_submarine_id;
}

bool Tourist::can_board_my_submarine(SystemInfo &sys_info) {
    int space_left = sys_info.get_submarine_capacity(my_submarine_id);
    int passenger_id;
    submarine_queues->mutex_lock();
    for (int position=0; position<submarine_queues->unsafe_get_size(my_submarine_id); position++) {
        passenger_id = submarine_queues->unsafe_get_tourist_id(my_submarine_id, position);
        space_left -= sys_info.get_tourist_size(passenger_id);
        if (passenger_id == id) break; // Chek only up to my position
    }
    submarine_queues->mutex_unlock();
    return (space_left >= 0);
}

bool Tourist::is_capitan() {
    return (id == submarine_queues->safe_get_tourist_id(my_submarine_id.load(), 0));
}

void Tourist::fill_boarded_on_my_submarine(SystemInfo &sys_info) {
    boarded_on_my_submarine.mutex_lock();
    boarded_on_my_submarine.unsafe_clear();
    int submarine_capacity_left = sys_info.get_submarine_capacity(my_submarine_id);
    int other_tourist_id, other_tourist_size;
    submarine_queues->mutex_lock();
    for (int position=0; position<submarine_queues->unsafe_get_size(my_submarine_id); position++) {
        other_tourist_id = submarine_queues->unsafe_get_tourist_id(my_submarine_id, position);
        other_tourist_size = sys_info.get_tourist_size(other_tourist_id);
        submarine_capacity_left -= other_tourist_size;
        if (submarine_capacity_left >= 0) {
            boarded_on_my_submarine.unsafe_push_back(other_tourist_id);
        } else {
            break; // The submarine is full
        }
    }
    submarine_queues->mutex_unlock();
    boarded_on_my_submarine.mutex_unlock();
}

void Tourist::add_to_queue_pony(int tourist_id) {
    if (std::find(queue_pony.begin(), queue_pony.end(), tourist_id) == queue_pony.end()) {
        queue_pony.push_back(tourist_id);
    }
}


void Tourist::fill_suplement_boarded_on_my_submarine(std::list<int> &list, int tourist_no) {
    for (int i=0; i<tourist_no; i++) {
        list.push_back(i);
    }
    boarded_on_my_submarine.mutex_lock();
    for (int i = 0; i < boarded_on_my_submarine.unsafe_get_size(); i++) {
        list.remove(boarded_on_my_submarine.unsafe_get_element(i));
    }
    boarded_on_my_submarine.mutex_unlock();
}


bool Tourist::is_submarine_deadlock(SystemInfo &sys_info) {
    submarine_queues->mutex_lock();
    bool is_deadlock = false;
    int tourists_in_submarines = 0;
    for (int i=0; i<sys_info.get_submarine_no(); i++) {
        tourists_in_submarines += submarine_queues->unsafe_get_size(i);
    }
    if (tourists_in_submarines == std::min(sys_info.get_tourist_no(), sys_info.get_pony_no())) {
        is_deadlock = true;
    }
    submarine_queues->mutex_unlock();
    return is_deadlock;
}
