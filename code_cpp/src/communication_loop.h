#pragma once

#include <memory>

#include "tourist.h"
#include "system_info.h"
#include "packet.h"


class CommunicationLoop {
public:
    CommunicationLoop(std::shared_ptr<Tourist> tourist, std::shared_ptr<SystemInfo> sys_info);
    ~CommunicationLoop();
    void run();

private:
    std::shared_ptr<Tourist> tourist;
    std::shared_ptr<SystemInfo> sys_info;
    bool run_flag;
    Packet packet;

    typedef void (CommunicationLoop::*cl_handler_t)();
    cl_handler_t lookup[12] = {
        &CommunicationLoop::handler_req_pony,
        &CommunicationLoop::handler_ack_pony,
        &CommunicationLoop::handler_req_submar,
        &CommunicationLoop::handler_ack_submar,
        &CommunicationLoop::handler_full_submar_retreat,
        &CommunicationLoop::handler_full_submar_stay,
        &CommunicationLoop::handler_return_submar,
        &CommunicationLoop::handler_travel_ready,
        &CommunicationLoop::handler_ack_travel,
        &CommunicationLoop::handler_depart_submar,
        &CommunicationLoop::handler_depart_submar_not_full,
        &CommunicationLoop::handler_kill_message
    };

    void handler_req_pony();
    void handler_ack_pony();
    void handler_req_submar();
    void handler_ack_submar();
    void handler_full_submar_stay();
    void handler_full_submar_retreat();
    void handler_return_submar();
    void handler_travel_ready();
    void handler_ack_travel();
    void handler_depart_submar();
    void handler_depart_submar_not_full();
    void handler_kill_message();

    void handler_wrong_message();
    void handler_wrong_state();
};
