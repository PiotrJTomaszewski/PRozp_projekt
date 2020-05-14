#pragma once

#include <vector>

class SystemInfo {
public:
    SystemInfo(int tourist_no);
    ~SystemInfo();
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
// TODO: Read those from file or sth
    int pony_no = 2;
    int tourist_no;
    int submar_no = 4;
    int max_try_no = 3;
    std::vector<int> dict_tourist_sizes = {3, 6, 8, 1, 3};
    std::vector<int> dict_submar_capacity = {12, 14, 26, 12, 31, 21};
    int min_rest_time = 1;
    int max_rest_time = 13;
    int min_travel_time = 3;
    int max_travel_time = 12;
};
