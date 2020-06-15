#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <list>

#include "main_loop.h"
#include "debug.h"
#include "packet.h"

MainLoop::MainLoop(std::shared_ptr<Tourist> tourist, std::shared_ptr<SystemInfo> sys_info) {
    this->tourist = tourist;
    this->sys_info = sys_info;
    run_flag = true;
    srand(time(NULL) + tourist->get_id());
}

MainLoop::~MainLoop() {

}

void MainLoop::run() {
    while (run_flag) {
        auto state = tourist->state.unsafe_get(); // State can change only be changed in main loop, so no need to lock
        if (state < Tourist::RESTING || state > Tourist::ON_SHORE) {
            handler_wrong_state();
        } else {
            auto func = lookup[static_cast<int>(state)];
            (this->*func)();
        }
    }
}

void MainLoop::handler_resting() {
    int resting_time = random_rest_time();
    Debug::dprintf(*tourist, "Going to sleep for %d seconds", resting_time);
    std::this_thread::sleep_for(std::chrono::seconds(resting_time));
    Debug::dprint(*tourist, "Woke up. Requesting a pony suit and changing state to WAIT_PONY");
    tourist->state.mutex_lock();
    tourist->received_ack_no = 1;
    tourist->my_req_pony_timestamp = Packet(Packet::REQ_PONY).broadcast(*tourist, sys_info->get_tourist_no());
    tourist->state.unsafe_set(Tourist::WAIT_PONY);
    tourist->state.mutex_unlock();
}

void MainLoop::handler_wait_pony() {
    int needed_ack = sys_info->get_tourist_no() - sys_info->get_pony_no() + 1;
    if (needed_ack <= 1) {
        Debug::dprint(*tourist, "There is enough pony suits for everyone, so I'm taking one");
    } else {
        tourist->cond_var.wait_for(ConditionVar::ENOUGH_ACK_SIGNAL);
        Debug::dprint(*tourist, "Received enough ACK_PONY, getting a suit and changing state to CHOOSE_SUBMAR");
    }
    tourist->state.safe_set(Tourist::CHOOSE_SUBMAR);
}

void MainLoop::handler_choose_submar() {
    int submarine_id = tourist->get_best_submarine_id(*sys_info);
    Debug::dprintf(*tourist, "I've chosen a submarine with id %d, asking for permission", submarine_id);
    tourist->my_submarine_id.store(submarine_id);
    tourist->state.mutex_lock();
    tourist->received_ack_no = 1;
    int req_timestamp = Packet(Packet::REQ_SUBMAR, submarine_id).broadcast(*tourist, sys_info->get_tourist_no());
    tourist->submarine_queues->safe_add(tourist->my_submarine_id.load(), tourist->get_id(), req_timestamp);
    Debug::dprint(*tourist, "Changing state to WAIT_SUBMAR");
    tourist->state.unsafe_set(Tourist::WAIT_SUBMAR);
    tourist->state.mutex_unlock();
}

void MainLoop::handler_wait_submar() {
    if (sys_info->get_tourist_no() == 1) {
        Debug::dprint(*tourist, "I'm the only tourist in the system and I have my own permission");
    } else {
        Debug::dprintf(*tourist, "Waiting for access to the %d submarine", tourist->my_submarine_id.load());
        tourist->cond_var.wait_for(ConditionVar::ALL_ACK_SIGNAL);
    }
    if (tourist->can_board_my_submarine(*sys_info)) {
        Debug::dprintf(*tourist, "Can board %d, changing state to BOARDED", tourist->my_submarine_id.load());
        tourist->state.safe_set(Tourist::BOARDED);
    } else {
        if (++try_no < sys_info->get_max_try_no() && (tourist->is_ack_travel_queued.load() == -1)) {
            tourist->available_submarine_list.safe_set_element(false, tourist->my_submarine_id.load());
            tourist->submarine_queues->safe_remove_tourist_id(tourist->my_submarine_id.load(), tourist->get_id());
            Debug::dprintf(*tourist, "Can't board %d, trying another one, so broadcasting FULL_SUBMARINE_RETREAT and changing state to CHOOSE_SUBMAR", tourist->my_submarine_id.load());
            tourist->state.mutex_lock();
            Packet(Packet::FULL_SUBMAR_RETREAT, tourist->my_submarine_id.load()).broadcast(*tourist, sys_info->get_tourist_no());
            tourist->state.unsafe_set(Tourist::CHOOSE_SUBMAR);
            tourist->state.mutex_unlock();
        } else {
            bool can_board = false;
            while (!can_board) {
                Debug::dprintf(*tourist, "Can't fit in %d but I've given up and decided to wait. Broadcasting FULL_SUBMARINE_STAY", tourist->my_submarine_id.load());
                Packet(Packet::FULL_SUBMAR_STAY, tourist->my_submarine_id.load()).broadcast(*tourist, sys_info->get_tourist_no());
                tourist->cond_var.wait_for(ConditionVar::MY_SUBMARINE_RETURNED_SIGNAL);
                if (tourist->can_board_my_submarine(*sys_info)) {
                    Debug::dprintf(*tourist, "The submarine %d has returned, boarding now", tourist->my_submarine_id.load());
                    can_board = true;
                } else {
                    Debug::dprintf(*tourist, "The submarine %d has returned, still can't fit", tourist->my_submarine_id.load());
                }
            }
            tourist->state.safe_set(Tourist::BOARDED);
        }
    }
}

void MainLoop::handler_boarded() {
    try_no = 0;
    int my_submarine_id = tourist->my_submarine_id.load();
    if (tourist->is_capitan()) {
        bool deadlock_detected = tourist->is_submarine_deadlock(*sys_info);
        if (!deadlock_detected) {
        Debug::dprintf(*tourist, "I'm a captain of %d, waiting for the submarine to get full", my_submarine_id);
        auto wait_val = tourist->cond_var.wait_for(ConditionVar::SUBMARINE_FULL_SIGNAL | ConditionVar::DEADLOCK_DETECTED_SIGNAL);
        deadlock_detected = ((wait_val & ConditionVar::DEADLOCK_DETECTED_SIGNAL) != 0);
        } else {
            Debug::dprint(*tourist, "Submarine deadlock detected!");
        }
        tourist->fill_boarded_on_my_submarine(*sys_info);
        int boarded_on_my_submar_no = tourist->boarded_on_my_submarine.safe_get_size();
        if (boarded_on_my_submar_no == 1) { 
            Debug::dprintf(*tourist, "Submarine %d is ready to depart and I'm alone, changing state to TRAVEL", my_submarine_id);
            tourist->state.safe_set(Tourist::TRAVEL);
        } else {
            Debug::dprintf(*tourist, "Submarine %d is ready to depart, asking for %d permissions to start a journey", my_submarine_id, boarded_on_my_submar_no-1);
            tourist->received_ack_no = 1;
            Packet(Packet::TRAVEL_READY).send_to_travelling_with_me(*tourist);
            tourist->cond_var.wait_for(ConditionVar::ALL_ACK_SIGNAL);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL");
            Packet::msg_t msg_type;
            if (deadlock_detected) {
                msg_type = Packet::DEPART_SUBMAR_NOT_FULL;
                Debug::dprintf(*tourist, "Informing %d other passengers on %d that submarines are leaving not full", boarded_on_my_submar_no-1, my_submarine_id);
            } else {
                msg_type = Packet::DEPART_SUBMAR;
                Debug::dprintf(*tourist, "Informing %d other passengers on %d that the submarine is leaving", boarded_on_my_submar_no-1, my_submarine_id);
            }
            tourist->received_ack_no = 1;
            Packet(msg_type).send_to_travelling_with_me(*tourist);
            Debug::dprint(*tourist, "Waiting for answers");
            tourist->cond_var.wait_for(ConditionVar::ALL_ACK_SIGNAL);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL, changing state to TRAVEL");
            tourist->state.safe_set(Tourist::TRAVEL);
        }
    } else {
        if (tourist->is_ack_travel_queued.load() == tourist->my_submarine_id.load()) {
            Debug::dprintf(*tourist, "Sending queued ACK_TRAVEL to the captain of %d", my_submarine_id);
            Packet(Packet::ACK_TRAVEL).send(*tourist, tourist->submarine_queues->safe_get_tourist_id(tourist->my_submarine_id, 0));
        }
        tourist->is_ack_travel_queued.store(-1);
        auto wait_var = tourist->cond_var.wait_for(ConditionVar::MY_SUBMARINE_RETURNED_SIGNAL | ConditionVar::JOURNEY_START_SIGNAL);
        if ((wait_var & ConditionVar::JOURNEY_START_SIGNAL) != 0) {
            Debug::dprint(*tourist, "Changing state to TRAVEL");
            tourist->state.safe_set(Tourist::TRAVEL);
        } else {
            // This can happen if deadlock was detected
            Debug::dprintf(*tourist, "My submarine (%d) has returned and I wasn't travelling", my_submarine_id);
        }
    }
}

void MainLoop::handler_travel() {
    int my_submarine_id = tourist->my_submarine_id.load();
    Debug::dprintf(*tourist, "Going on a journey on the submarine %d", my_submarine_id);
    if (tourist->is_capitan()) {
        int travel_time = random_travel_time();
        Debug::dprintf(*tourist, "Journey on %d will take %d seconds", my_submarine_id, travel_time);
        std::this_thread::sleep_for(std::chrono::seconds(travel_time));
        int on_my_submarine_size = tourist->boarded_on_my_submarine.safe_get_size();
        if (on_my_submarine_size > 1) {
            Debug::dprintf(*tourist, "Journey on %d has ended, informing %d other passengers", my_submarine_id, on_my_submarine_size-1);
            tourist->received_ack_no = 1;
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, on_my_submarine_size).send_to_travelling_with_me(*tourist);
            Debug::dprint(*tourist, "Waiting for all of the ACK_TRAVEL responses");
            tourist->cond_var.wait_for(ConditionVar::ALL_ACK_SIGNAL);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL responses, informing other tourists");
            std::list<int> to_send_list;
            tourist->fill_suplement_boarded_on_my_submarine(to_send_list, sys_info->get_tourist_no());
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, on_my_submarine_size).send(*tourist, to_send_list);
        } else {
            Debug::dprintf(*tourist, "Journey on %d has ended, there is no one with me on board. Informing all about the fact", my_submarine_id);
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, 1).broadcast(*tourist, sys_info->get_tourist_no());
        }
        tourist->submarine_queues->safe_remove_from_begin(my_submarine_id, on_my_submarine_size);
    } else {
        Debug::dprint(*tourist, "Waiting for the journey to end");
        tourist->cond_var.wait_for(ConditionVar::JOURNEY_END_SIGNAL);
    }
    Debug::dprint(*tourist, "Getting on shore");
    tourist->state.safe_set(Tourist::ON_SHORE);
}

void MainLoop::handler_on_shore() {
    int acks_to_send = tourist->queue_pony.size();
    if (acks_to_send > 0) {
        Packet packet(Packet::ACK_PONY);
        Debug::dprintf(*tourist, "Sending ACK_PONY to %d processes", acks_to_send);
        for (int i = 0; i < acks_to_send; i++) {
            packet.send(*tourist, tourist->queue_pony.back());
            tourist->queue_pony.pop_back();
        }
    }
    Debug::dprint(*tourist, "Going to sleep");
    tourist->state.safe_set(Tourist::RESTING);
}

void MainLoop::handler_wrong_state() {
    Debug::dprintf(*tourist, "Unknown state: %d, terminating!", tourist->state.safe_get());
    run_flag = false;
}


int MainLoop::random_rest_time() {
    return sys_info->get_min_rest_time() + rand() % (sys_info->get_max_rest_time() - sys_info->get_min_rest_time() + 1);
}

int MainLoop::random_travel_time() {
    return sys_info->get_min_travel_time() + rand() % (sys_info->get_max_travel_time() - sys_info->get_min_travel_time() + 1);
}