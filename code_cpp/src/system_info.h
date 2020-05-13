#pragma once

#include <vector>

class SystemInfo {
public:
    SystemInfo();
    int get_pony_no();
    int get_tourist_no();
    int get_submarine_no();
    int get_max_try_no();
    int get_tourist_size(int tourist_id);
    int get_submarine_capacity(int submarine_id);
    int get_min_rest_time();
    int get_max_rest_time();
    int get_min_travel_time();
    int get_max_travel_time();

private:
    int pony_no = 6;
    int tourist_no = 3;
    int submar_no = 4;
    int max_try_no = 3;
    std::vector<int> dict_tourist_sizes;
    std::vector<int> dict_submar_capacity;
    int min_rest_time = 1;
    int max_rest_time = 13;
    int min_travel_time = 3;
    int max_travel_time = 12;
};
