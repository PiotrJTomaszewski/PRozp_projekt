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

    void handler_resting();
    void handler_wait_pony();
    void handler_choose_submar();
    void handler_wait_submar();
    void handler_boarded();
    void handler_travel();
    void handler_on_shore();
    void handler_wrong_state();

    typedef void (MainLoop::*ml_handler_t)();
    ml_handler_t lookup[7] = {
        &MainLoop::handler_resting,
        &MainLoop::handler_wait_pony,
        &MainLoop::handler_choose_submar,
        &MainLoop::handler_wait_submar,
        &MainLoop::handler_boarded,
        &MainLoop::handler_travel,
        &MainLoop::handler_on_shore
    };

    int random_rest_time();
    int random_travel_time();

    int try_no = 0;
};
