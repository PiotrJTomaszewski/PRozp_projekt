#include <list>
#include "tourist.h"

Tourist::Tourist(int id, int submarine_no) {
    this->id = id;
    submarine_queues = std::unique_ptr<QueuesSubmarine>(new QueuesSubmarine(submarine_no));
}

Tourist::~Tourist() {

}

int Tourist::get_id() {
    return id;
}

int Tourist::increment_try_no() {
    return try_no++;
}

int Tourist::get_try_no() {
    return try_no;
}

void Tourist::set_try_no(int value) {
    try_no = value;
}

int Tourist::get_best_submarine_id(SystemInfo &sys_info) {
    int submarine_capacity;
    int submarine_capacity_left;
    int empty_submarine_id = -1;
    int best_submarine_id = -1;
    int submarine_queue_size;
    double current_submarine_capacity_taken;
    double best_submarine_capacity_taken = 1.f;
    available_submarine_list.mutex_lock();
    submarine_queues->mutex_lock();
    /* Going in reverse because we want to unsafe_get the submarine with the lowest id
    and this way we don't have to make extra checks on empty boats*/
    for (int submarine_id=sys_info.get_submarine_no()-1; submarine_id>=0; submarine_id--) {
        if (!available_submarine_list.unsafe_get_element(submarine_id))
            continue;
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
    return best_submarine_id;
}

bool Tourist::can_board(SystemInfo &sys_info) {
    int space_left = sys_info.get_submarine_capacity(my_submarine_id);
    submarine_queues->mutex_lock();
    for (int position=0; position<submarine_queues->unsafe_get_size(my_submarine_id); position++) {
        space_left -= sys_info.get_tourist_size(submarine_queues->unsafe_get_tourist_id(my_submarine_id, position));
    }
    submarine_queues->mutex_unlock();
    return (space_left >= sys_info.get_tourist_size(get_id()));
}

bool Tourist::is_capitan() {
    return (get_id() == submarine_queues->safe_get_tourist_id(my_submarine_id, 0));
}

void Tourist::fill_boarded_on_my_submarine(SystemInfo &sys_info) {
    boarded_on_my_submarine.clear();
    int submarine_capacity_left = sys_info.get_submarine_capacity(my_submarine_id);
    int other_tourist_id, other_tourist_size;
    submarine_queues->mutex_lock();
    for (int position=0; position<submarine_queues->unsafe_get_size(my_submarine_id); position++) {
        other_tourist_id = submarine_queues->unsafe_get_tourist_id(my_submarine_id, position);
        other_tourist_size = sys_info.get_tourist_size(other_tourist_id);
        submarine_capacity_left -= other_tourist_size;
        if (submarine_capacity_left >= 0) {
            boarded_on_my_submarine.push_back(other_tourist_id);
        } else {
            break; // The submarine is full
        }
    }
    submarine_queues->mutex_unlock();
}

int Tourist::get_boarded_on_my_submarine_size() {
    return boarded_on_my_submarine.size();
}

void Tourist::fill_suplement_boarded_on_my_submarine(std::list<int> &list, int tourist_no) {
    for (int i=0; i<tourist_no; i++) {
        list.push_back(i);
    }
    for (auto it = boarded_on_my_submarine.begin(); it != boarded_on_my_submarine.end(); ++it) {
        list.remove(*it);
    }
}