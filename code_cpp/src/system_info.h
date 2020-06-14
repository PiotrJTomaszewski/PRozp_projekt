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
    int tourist_no; // Will be set automatically
    const int pony_no = 25;
    const int submar_no = 6;
    const int max_try_no = 3;
                                                 //0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
    const std::vector<int> dict_tourist_sizes   = {3,  6,  8,  1,  3,  6,  8,  9,  2,  4,  5,  7, 8, 9, 1, 2, 3,  4,  5,  9,  10, 2,  7,  1,  8,  10, 10, 7,  5,  4,  2,  6};
    const std::vector<int> dict_submar_capacity = {11, 14, 16, 12, 31, 21, 12, 20, 11, 12, 14};
    const int min_rest_time = 1;
    const int max_rest_time = 5;
    const int min_travel_time = 1;
    const int max_travel_time = 4;
};
