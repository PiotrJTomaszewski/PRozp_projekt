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

void MainLoop::lock_if_needed(std::unique_lock<std::mutex> &mutex) {
    if (!mutex.owns_lock()) {
        mutex.lock();
    }
}


void MainLoop::run() {
    while (run_flag) {
        switch (tourist->state.safe_get()) { // TODO: Maybe change to lookup?
        case Tourist::RESTING:
            handler_resting();
            break;
        case Tourist::WAIT_PONY:
            handler_wait_pony();
            break;
        case Tourist::CHOOSE_SUBMAR:
            handler_choose_submar();
            break;
        case Tourist::WAIT_SUBMAR:
            handler_wait_submar();
            break;
        case Tourist::BOARDED:
            handler_boarded();
            break;
        case Tourist::TRAVEL:
            handler_travel();
            break;
        case Tourist::ON_SHORE:
            handler_on_shore();
            break;
        default:
            handler_wrong_state();
            break;
        }
    }
}

void MainLoop::handler_resting() {
    int resting_time = random_rest_time();
    Debug::dprintf(*tourist, "Going to sleep for %d seconds", resting_time);
    std::this_thread::sleep_for(std::chrono::seconds(resting_time));
    Debug::dprint(*tourist, "Woke up. Requesting a pony suit and changing state to WAIT_PONY");
    tourist->state.mutex_lock();
    tourist->clear_received_ack_no();
    tourist->my_req_pony_timestamp = Packet(Packet::REQ_PONY).broadcast(*tourist, sys_info->get_tourist_no());
    tourist->state.unsafe_set(Tourist::WAIT_PONY);
    tourist->state.mutex_unlock();
}

void MainLoop::handler_wait_pony() {
    int needed_ack = sys_info->get_tourist_no() - sys_info->get_pony_no() + 1;
    if (needed_ack <= 1) {
        Debug::dprint(*tourist, "There is enough pony suits for everyone, so I'm taking one");
    } else {
        tourist->ack_pony_condition.wait_for(ConditionVar::ENOUGH_ACK);
        Debug::dprint(*tourist, "Received enough ACK_PONY, getting a suit and changing state to CHOOSE_SUBMAR");
    }
    tourist->state.safe_set(Tourist::CHOOSE_SUBMAR);
}

void MainLoop::handler_choose_submar() {
    int submarine_id = tourist->get_best_submarine_id(*sys_info);
    if (submarine_id >= 0) {
        Debug::dprintf(*tourist, "I've chosen a submarine with id %d, asking for permission", submarine_id);
        tourist->my_submarine_id = submarine_id;
        tourist->state.mutex_lock();
        tourist->clear_received_ack_no();
        int req_timestamp = Packet(Packet::REQ_SUBMAR, submarine_id).broadcast(*tourist, sys_info->get_tourist_no());
        tourist->submarine_queues->safe_add(tourist->my_submarine_id.load(), tourist->get_id(), req_timestamp);
        Debug::dprint(*tourist, "Changing state to WAIT_SUBMAR");
        tourist->state.unsafe_set(Tourist::WAIT_SUBMAR);
        tourist->state.mutex_unlock();
    } else {
        Debug::dprint(*tourist, "No free submarine found, waiting for one to return");
        tourist->submarine_return_condition.wait_for(ConditionVar::ANY_SUBMARINE);
        Debug::dprint(*tourist, "I've noticed that a submarine has returned, waking up"); // TODO: Tourist HAS TO select this submarine
    }
}

void MainLoop::handler_wait_submar() {
    if (sys_info->get_tourist_no() == 1) {
        Debug::dprint(*tourist, "I'm the only tourist in the system and I have my own permission");
    } else {
        Debug::dprintf(*tourist, "Waiting for access to the %d submarine", tourist->my_submarine_id.load());
        tourist->ack_submar_condition.wait_for(ConditionVar::ALL_ACK);
    }
    if (tourist->can_board_my_submarine(*sys_info)) {
        Debug::dprintf(*tourist, "Can board %d, changing state to BOARDED", tourist->my_submarine_id.load());
        tourist->state.safe_set(Tourist::BOARDED);
    } else {
        if (tourist->increment_try_no() < sys_info->get_max_try_no()) {
            tourist->available_submarine_list.safe_set_element(false, tourist->my_submarine_id.load());
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
                tourist->submarine_return_condition.wait_for(ConditionVar::MY_SUBMARINE);
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
    int my_submarine_id = tourist->my_submarine_id.load();
    if (tourist->is_capitan()) { // TODO: Sometimes he can be that last passenger
        Debug::dprintf(*tourist, "I'm a captain of %d, waiting for the submarine to get full", my_submarine_id);
        auto wait_val = tourist->submarine_should_leave_condition.wait();
        bool deadlock_detected = (wait_val == ConditionVar::DEADLOCK_DETECTED);
        tourist->fill_boarded_on_my_submarine(*sys_info);
        if (tourist->get_boarded_on_my_submarine_size() == 1) {
            Debug::dprintf(*tourist, "Submarine %d is full and I'm alone, changing state to TRAVEL", my_submarine_id);
            tourist->state.safe_set(Tourist::TRAVEL);
        } else {
            Debug::dprintf(*tourist, "Submarine %d is full, asking for permission to start a journey", my_submarine_id);
            tourist->clear_received_ack_no();
            Packet(Packet::TRAVEL_READY).send_to_travelling_with_me(*tourist);
            Debug::dprint(*tourist, "Waiting for answers");
            tourist->ack_travel_condition.wait_for(ConditionVar::ALL_ACK);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL");
            Packet::msg_t msg_type;
            if (deadlock_detected) {
                msg_type = Packet::DEPART_SUBMAR_NOT_FULL;
                Debug::dprintf(*tourist, "Informing passengers on %d that submarines are leaving not full", my_submarine_id);
            } else {
                msg_type = Packet::DEPART_SUBMAR;
                Debug::dprintf(*tourist, "Informing passengers on %d that the submarine is leaving", my_submarine_id);
            }
            tourist->clear_received_ack_no();
            Packet(msg_type).send_to_travelling_with_me(*tourist);
            Debug::dprint(*tourist, "Waiting for answers");
            tourist->ack_travel_condition.wait_for(ConditionVar::ALL_ACK);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL, changing state to TRAVEL");
            tourist->state.safe_set(Tourist::TRAVEL);
        }
    } else {
        if (tourist->get_and_clear_is_ack_travel_queued()) {
            Debug::dprintf(*tourist, "Sending queued ACK_TRAVEL to the captain of %d", my_submarine_id);
            Packet(Packet::ACK_TRAVEL).send(*tourist, tourist->my_submarine_captain_id.load());
        }
        tourist->submarine_depart_condition.wait_for(ConditionVar::MY_SUBMARINE);
        Debug::dprint(*tourist, "Changing state to TRAVEL");
        tourist->state.safe_set(Tourist::TRAVEL);
    }
}

void MainLoop::handler_travel() {
    int my_submarine_id = tourist->my_submarine_id.load();
    Debug::dprintf(*tourist, "Going on a journey on the submarine %d", my_submarine_id);
    if (tourist->is_capitan()) {
        int travel_time = random_travel_time();
        Debug::dprintf(*tourist, "Journey on %d will take %d seconds", my_submarine_id, travel_time);
        std::this_thread::sleep_for(std::chrono::seconds(travel_time));
        int on_my_submarine = tourist->get_boarded_on_my_submarine_size();
        if (on_my_submarine > 1) {
            Debug::dprintf(*tourist, "Journey on %d has ended, informing passengers", my_submarine_id);
            tourist->clear_received_ack_no();
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, on_my_submarine).send_to_travelling_with_me(*tourist);
            Debug::dprint(*tourist, "Waiting for all of the ACK_TRAVEL responses");
            tourist->ack_travel_condition.wait_for(ConditionVar::ALL_ACK);
            Debug::dprint(*tourist, "Received all of the ACK_TRAVEL responses");
            std::list<int> to_send_list;
            tourist->fill_suplement_boarded_on_my_submarine(to_send_list, sys_info->get_tourist_no());
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, on_my_submarine).send(*tourist, to_send_list);
        } else {
            Debug::dprintf(*tourist, "Journey on %d has ended, there is no one with me on board. Informing all about the fact", my_submarine_id);
            Packet(Packet::RETURN_SUBMAR, my_submarine_id, on_my_submarine).broadcast(*tourist, sys_info->get_tourist_no());
        }
        tourist->submarine_queues->safe_remove_from_begin(my_submarine_id, on_my_submarine);
    } else {
        Debug::dprint(*tourist, "Waiting for the journey to end");
        tourist->submarine_return_condition.wait_for(ConditionVar::MY_SUBMARINE);
    }
    Debug::dprint(*tourist, "Getting on shore");
    tourist->state.safe_set(Tourist::ON_SHORE);
}

void MainLoop::handler_on_shore() {
    tourist->queue_pony.mutex_lock();
    int acks_to_send = tourist->queue_pony.unsafe_get_size();
    if (acks_to_send > 0) {
        Packet packet(Packet::ACK_PONY);
        Debug::dprintf(*tourist, "Sending ACK_PONY to %d processes", acks_to_send);
        for (int i = 0; i < acks_to_send; i++) {
            packet.send(*tourist, tourist->queue_pony.unsafe_pop());
        }
    }
    tourist->queue_pony.mutex_unlock();
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