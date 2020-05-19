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
    // std::random_device rd;
    // std::mt19937 mt(rd());
    // random_rest_time = std::uniform_int_distribution<int>(sys_info->get_min_rest_time(), sys_info->get_max_rest_time());
    // random_travel_time = std::uniform_int_distribution<int>(sys_info->get_min_travel_time(), sys_info->get_max_travel_time());

    // Only this thread will wait on conditions, so we can lock them all at the beginning. Just to make sure we won't loose any signals.
    ack_pony_cond_unqiue_mutex = tourist->ack_pony_condition.mutex_lock();
    ack_submar_cond_unique_mutex = tourist->ack_submar_condition.mutex_lock();
    submarine_return_cond_unique_mutex = tourist->submarine_return_condition.mutex_lock();
    full_submarine_cond_unique_mutex = tourist->full_submarine_condition.mutex_lock();
    ack_travel_cond_unique_mutex = tourist->ack_travel_condition.mutex_lock();
    travel_cond_unique_mutex = tourist->travel_condition.mutex_lock();
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
    Debug::dprintf(Debug::INFO_OTHER, *tourist, "Going to sleep for %d seconds", resting_time);
    std::this_thread::sleep_for(std::chrono::seconds(resting_time));
    Debug::dprint(Debug::INFO_SENDING, *tourist, "Woke up. Requesting a pony suit");

    lock_if_needed(ack_pony_cond_unqiue_mutex);
    // last_cond_mutex = tourist->ack_pony_condition.mutex_lock(); // Locking, because we wan't to make sure we get all of the ack
    tourist->my_req_pony_timestamp = Packet(Packet::REQ_PONY).broadcast(*tourist, sys_info->get_tourist_no());
    Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Changing state to WAIT_PONY");
    tourist->state.safe_set(Tourist::WAIT_PONY);
}

void MainLoop::handler_wait_pony() {
    int needed_ack = sys_info->get_tourist_no() - sys_info->get_pony_no() + 1;
    if (needed_ack <= 1) {
        Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "There is enough pony suits for everyone, so I'm taking one");
        // ack_pony_cond_unqiue_mutex.unlock(); // We don't have to wait for a signal
    } else {
        tourist->ack_pony_condition.wait_no_relock(ack_pony_cond_unqiue_mutex);
        Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Received enough ACK_PONY, getting a suit and changing state to CHOOSE_SUBMAR");
    }
    // last_cond_mutex = tourist->submarine_return_condition.mutex_lock();
    tourist->state.safe_set(Tourist::CHOOSE_SUBMAR);
}

void MainLoop::handler_choose_submar() {
    int submarine_id = tourist->get_best_submarine_id(*sys_info);
    if (submarine_id >= 0) {
        // if (last_cond_mutex.owns_lock()) last_cond_mutex.unlock();
        Debug::dprintf(Debug::INFO_OTHER, *tourist, "I've chosen a submarine with id %d, asking for permission", submarine_id);
        tourist->my_submarine_id = submarine_id;
        // last_cond_mutex = tourist->ack_submar_condition.mutex_lock(); // Locking to prevent missing responses
        int req_timestamp = Packet(Packet::REQ_SUBMAR, submarine_id).broadcast(*tourist, sys_info->get_tourist_no());
        tourist->submarine_queues->safe_add(tourist->my_submarine_id.load(), tourist->get_id(), req_timestamp);
        Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Changing state to WAIT_SUBMAR");
        tourist->state.safe_set(Tourist::WAIT_SUBMAR);
    } else {
        Debug::dprint(Debug::INFO_WAITING, *tourist, "No free submarine found, waiting for one to return");
        tourist->submarine_return_condition.wait(submarine_return_cond_unique_mutex);
        Debug::dprint(Debug::INFO_OTHER, *tourist, "I've noticed that a submarine has returned, waking up"); // TODO: Tourist HAS TO select this submarine
    }
}

void MainLoop::handler_wait_submar() {
    if (sys_info->get_tourist_no() == 1) {
        Debug::dprint(Debug::INFO_OTHER, *tourist, "I'm the only tourist in the system and I have my own permission");
        // last_cond_mutex.unlock();
    } else {
        Debug::dprintf(Debug::INFO_OTHER, *tourist, "Waiting for access to the %d submarine", tourist->my_submarine_id.load());
        tourist->ack_submar_condition.wait(ack_submar_cond_unique_mutex);
    }
    // if (tourist->can_board(*sys_info)) {
    //     Debug::dprintf(Debug::INFO_CHANGE_STATE, *tourist, "Can board %d, changing state to BOARDED", tourist->my_submarine_id.load());
    //     tourist->state.safe_set(Tourist::BOARDED);
    // } else {
    //     if (tourist->increment_try_no() < sys_info->get_max_try_no()) {
    //         tourist->available_submarine_list.safe_set_element(false, tourist->my_submarine_id.load());
    //         Debug::dprintf(Debug::INFO_CHANGE_STATE, *tourist, "Can't board %d, trying another one, so changing state to CHOOSE_SUBMAR");
    //         Debug::dprint(Debug::INFO_SENDING, *tourist, "Broadcasting FULL_SUBMARINE_RETREAT");
    //         Packet(Packet::FULL_SUBMAR_RETREAT, tourist->my_submarine_id.load()).broadcast(*tourist, sys_info->get_tourist_no());
    //         tourist->state.safe_set(Tourist::CHOOSE_SUBMAR);
    //     } else {
    //         Debug::dprintf(Debug::INFO_WAITING, *tourist, "Can't fit in %d but I've given up and decided to wait", tourist->my_submarine_id.load());
    //         Debug::dprint(Debug::INFO_SENDING, *tourist, "Broadcasting FULL_SUBMARINE_STAY");
    //         Packet(Packet::FULL_SUBMAR_STAY, tourist->my_submarine_id.load()).broadcast(*tourist, sys_info->get_tourist_no());
    //         tourist->submarine_return_condition.wait(submarine_return_cond_unique_mutex);
    //         Debug::dprintf(Debug::INFO_WAITING, *tourist, "The submaring %d has returned, boarding now", tourist->my_submarine_id.load());
    //         tourist->state.safe_set(Tourist::BOARDED);
    //     }
    // }
}

void MainLoop::handler_boarded() {
    if (tourist->is_capitan()) { // TODO: Sometimes he can be that last passenger
        // last_cond_mutex = tourist->full_submarine_condition.mutex_lock();
        Debug::dprintf(Debug::INFO_WAITING, *tourist, "I'm a captain of %d, waiting for the submarine to get full", tourist->my_submarine_id.load());
        tourist->full_submarine_condition.wait(full_submarine_cond_unique_mutex); // TODO: Rename to sth like submarine ready to travel
        tourist->fill_boarded_on_my_submarine(*sys_info);
        if (tourist->get_boarded_on_my_submarine_size() == 1) {
            Debug::dprintf(Debug::INFO_OTHER, *tourist, "Submarine %d is full and I'm alone", tourist->my_submarine_id.load());
        } else {
            Debug::dprintf(Debug::INFO_SENDING, *tourist, "Submarine %d is full, asking for permission to start the travel", tourist->my_submarine_id.load());
            // last_cond_mutex = tourist->ack_travel_condition.mutex_lock();
            Packet(Packet::ACK_TRAVEL).send_to_travelling_with_me(*tourist);
            Debug::dprint(Debug::INFO_WAITING, *tourist, "Waiting for answers");
            tourist->ack_travel_condition.wait(ack_travel_cond_unique_mutex);
            Debug::dprint(Debug::INFO_OTHER, *tourist, "Received all of the ACK_TRAVEL");
            Packet::msg_t msg_type;
            if (tourist->was_submarine_deadlock_detected.load()) {
                msg_type = Packet::DEPART_SUBMAR_NOT_FULL;
                Debug::dprintf(Debug::INFO_SENDING, *tourist, "Informing passengers on %d that submarines are leaving not full", tourist->my_submarine_id.load());
            } else {
                msg_type = Packet::DEPART_SUBMAR;
                Debug::dprintf(Debug::INFO_SENDING, *tourist, "Informing passengers on %d that the submarine is leaving", tourist->my_submarine_id.load());
            }
            // last_cond_mutex = tourist->travel_condition.mutex_lock();
            Packet(msg_type).send_to_travelling_with_me(*tourist);
            Debug::dprint(Debug::INFO_WAITING, *tourist, "Waiting for answers");
            tourist->travel_condition.wait(travel_cond_unique_mutex);
            Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Received all of the ACK_TRAVEL, changing state to TRAVEL");
            tourist->state.safe_set(Tourist::TRAVEL);
        }
    } else {
        // last_cond_mutex = tourist->travel_condition.mutex_lock(); // TODO: Maybe it should be added earlier
        if (tourist->get_and_clear_is_ack_travel_queued()) {
            Debug::dprintf(Debug::INFO_SENDING, *tourist, "Sending queued ACK_TRAVEL to the captain of %d", tourist->my_submarine_id.load());
            Packet(Packet::ACK_TRAVEL).send(*tourist, tourist->my_submarine_get_captain_id());
        }
        tourist->travel_condition.wait(travel_cond_unique_mutex);
        Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Changing state to TRAVEL");
        // last_cond_mutex = tourist->submarine_return_condition.mutex_lock();
        tourist->state.safe_set(Tourist::TRAVEL);
    }
}

void MainLoop::handler_travel() {
    Debug::dprintf(Debug::INFO_OTHER, *tourist, "Going on a journey on the submarine %d", tourist->my_submarine_id.load());
    if (tourist->is_capitan()) {
        int travel_time = random_travel_time();
        Debug::dprintf(Debug::INFO_OTHER, *tourist, "Journey on %d will take %d seconds", travel_time);
        std::this_thread::sleep_for(std::chrono::seconds(travel_time));
        // last_cond_mutex = tourist->ack_travel_condition.mutex_lock();
        int on_my_submarine = tourist->get_boarded_on_my_submarine_size();
        Debug::dprintf(Debug::INFO_SENDING, *tourist, "Journey on %d has ended, informing passengers", tourist->my_submarine_id.load());
        Packet(Packet::RETURN_SUBMAR, tourist->my_submarine_id, on_my_submarine).send_to_travelling_with_me(*tourist);
        Debug::dprint(Debug::INFO_WAITING, *tourist, "Waiting for all of the ACK_TRAVEL responses");
        tourist->submarine_queues->safe_remove_from_begin(tourist->my_submarine_id, on_my_submarine);
        tourist->ack_travel_condition.wait(ack_travel_cond_unique_mutex);
        Debug::dprint(Debug::INFO_OTHER, *tourist, "Received all of the ACK_TRAVEL responses");
        std::list<int> to_send_list;
        tourist->fill_suplement_boarded_on_my_submarine(to_send_list, sys_info->get_tourist_no());
        Packet(Packet::RETURN_SUBMAR, tourist->my_submarine_id, on_my_submarine).send(*tourist, to_send_list);
    } else {
        Debug::dprint(Debug::INFO_WAITING, *tourist, "Waiting for the journey to end");
        tourist->submarine_return_condition.wait(submarine_return_cond_unique_mutex);
        int captain_id = tourist->submarine_queues->safe_get_tourist_id(tourist->my_submarine_id, 0);
        Debug::dprintf(Debug::INFO_SENDING, *tourist, "Responing ACK_TRAVEL to the captain (%d)", captain_id);
        Packet(Packet::ACK_TRAVEL).send(*tourist, captain_id);
    }
    Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Getting on shore");
    tourist->state.safe_set(Tourist::ON_SHORE);
}

void MainLoop::handler_on_shore() {
    tourist->queue_pony.mutex_lock();
    int acks_to_send = tourist->queue_pony.unsafe_get_size();
    if (acks_to_send > 0) {
        Packet packet(Packet::ACK_PONY);
        Debug::dprintf(Debug::INFO_SENDING, *tourist, "Sending ACK_PONY to %d processes", acks_to_send);
        for (int i = 0; i < acks_to_send; i++) {
            packet.send(*tourist, tourist->queue_pony.unsafe_pop());
        }
    }
    tourist->queue_pony.mutex_unlock();
    Debug::dprint(Debug::INFO_CHANGE_STATE, *tourist, "Going to sleep");
    tourist->state.safe_set(Tourist::RESTING);
}

void MainLoop::handler_wrong_state() {
    Debug::dprintf(Debug::ERROR_OTHER, *tourist, "Unknown state: %d, terminating!", tourist->state.safe_get());
    run_flag = false;
}


int MainLoop::random_rest_time() {
    return sys_info->get_min_rest_time() + rand() % (sys_info->get_max_rest_time() - sys_info->get_min_rest_time() + 1);
}

int MainLoop::random_travel_time() {
    return sys_info->get_min_travel_time() + rand() % (sys_info->get_max_travel_time() - sys_info->get_min_travel_time() + 1);
}