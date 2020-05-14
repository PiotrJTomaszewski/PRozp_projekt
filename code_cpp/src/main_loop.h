#pragma once

#include <random>
#include <memory>
#include <mutex>

#include "tourist.h"
#include "system_info.h"

class MainLoop {
public:
    MainLoop(std::shared_ptr<Tourist> tourist, std::shared_ptr<SystemInfo> sys_info);
    ~MainLoop();
    void run();

private:
    std::shared_ptr<Tourist> tourist;
    std::shared_ptr<SystemInfo> sys_info;
    bool run_flag;
    inline void handler_resting();
    inline void handler_wait_pony();
    inline void handler_choose_submar();
    inline void handler_wait_submar();
    inline void handler_boarded();
    inline void handler_travel();
    inline void handler_on_shore();
    inline void handler_wrong_state();

    int random_rest_time();
    int random_travel_time();

    std::unique_lock<std::mutex> last_cond_mutex;

    // std::mt19937 pseudo_random_generator;
    // std::uniform_int_distribution<int> random_rest_time;
    // std::uniform_int_distribution<int> random_travel_time;
};
