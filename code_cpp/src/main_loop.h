#pragma once

#include <random>
#include <memory>

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
    void handler_resting();
    void handler_wait_pony();
    void handler_choose_submar();
    void handler_wait_submar();
    void handler_boarded();
    void handler_travel();
    void handler_on_shore();
    void handler_wrong_state();

    std::mt19937 pseudo_random_generator;
    std::uniform_int_distribution<int> random_rest_time;
    std::uniform_int_distribution<int> random_travel_time;
};
