#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include <list>

#include "shared_var.h"
#include "condition_var.h"
#include "system_info.h"
#include "shared_vector.h"
#include "queues_submarine.h"

class Tourist {
public:
    Tourist(int id, int submarine_no);
    ~Tourist();
    enum state_t {
        RESTING, WAIT_PONY, CHOOSE_SUBMAR,
        WAIT_SUBMAR, BOARDED, TRAVEL, ON_SHORE
    };
    SharedVar<state_t> state;
    std::atomic<int> lamport_clock;
    std::atomic<int> my_submarine_id;
    std::atomic<bool> is_my_submarine_full;
    SharedVector<bool> available_submarine_list;
    SharedVector<int> boarded_on_my_submarine;
    std::unique_ptr<QueuesSubmarine> submarine_queues;
    ConditionVar cond_var;

    std::atomic<int> my_req_pony_timestamp;
    std::atomic<int> is_ack_travel_queued; // if queued then submarine_id, -1 if not
    std::vector<int> queue_pony;
    void add_to_queue_pony(int tourist_id);
    std::atomic<int> received_ack_no;

    int get_id();

    int get_best_submarine_id(SystemInfo &sys_info);
    bool can_board_my_submarine(SystemInfo &sys_info);
    bool is_capitan();
    void fill_boarded_on_my_submarine(SystemInfo &sys_info);
    void fill_suplement_boarded_on_my_submarine(std::list<int> &list, int tourist_no);
    std::atomic<int> my_submarine_captain_id;

    bool is_submarine_deadlock(SystemInfo &sys_info);


private:
    int id;
    int ack_travel_queued_submarine_id;
};
