#include "system_info.h"

SystemInfo::SystemInfo(int tourist_no) {
    this->tourist_no = tourist_no;
}

SystemInfo::~SystemInfo() {
    
}

int SystemInfo::get_pony_no() {
    return pony_no;
}

int SystemInfo::get_tourist_no() {
    return tourist_no;
}

int SystemInfo::get_submarine_no() {
    return submar_no;
}

int SystemInfo::get_max_try_no() {
    return max_try_no;

}
int SystemInfo::get_tourist_size(int tourist_id) {
    return dict_tourist_sizes[tourist_id];
}

int SystemInfo::get_submarine_capacity(int submarine_id) {
    return dict_submar_capacity[submarine_id];
}

int SystemInfo::get_min_rest_time() {
    return min_rest_time;
}

int SystemInfo::get_max_rest_time() {
    return max_rest_time;
}

int SystemInfo::get_min_travel_time() {
    return min_travel_time;
}

int SystemInfo::get_max_travel_time() {
    return max_travel_time;
}
