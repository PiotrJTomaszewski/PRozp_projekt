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
    SharedVar<state_t> state; // See if can change to atomic
    std::atomic<int> lamport_clock;
    std::atomic<int> my_req_pony_timestamp;
    std::atomic<int> my_submarine_id;
    std::atomic<bool> is_my_submarine_full;
    SharedVector<bool> available_submarine_list;
    SharedVector<int> queue_pony;
    std::unique_ptr<QueuesSubmarine> submarine_queues;
    ConditionVar ack_pony_condition;
    ConditionVar ack_submar_condition;
    ConditionVar submarine_return_condition;
    ConditionVar full_submarine_condition;
    ConditionVar ack_travel_condition;
    ConditionVar travel_condition;
    int get_id();
    int increment_try_no();
    int get_try_no();
    void set_try_no(int value);
    int increment_received_ack_no();
    void clear_received_ack_no();

    int get_best_submarine_id(SystemInfo &sys_info);
    bool can_board(SystemInfo &sys_info);
    bool is_capitan();
    void fill_boarded_on_my_submarine(SystemInfo &sys_info);
    int get_boarded_on_my_submarine_size();
    void fill_suplement_boarded_on_my_submarine(std::list<int> &list, int tourist_no);
    int my_submarine_get_captain_id();

    void queue_ack_travel(); // TODO: Protect from race condition?
    bool get_and_clear_is_ack_travel_queued();

    bool is_submarine_deadlock(int tourist_no);
    std::atomic<bool> was_submarine_deadlock_detected;

private:
    int id;
    std::vector<int> boarded_on_my_submarine;
    int try_no; // Only main loop'll have access to this
    int received_ack_no; // Only communication loop'll have access to this
    bool is_ack_travel_queued;
    int  ack_travel_queued_submarine_id;
};
