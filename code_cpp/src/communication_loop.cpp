#include "communication_loop.h"
#include "packet.h"
#include "debug.h"

CommunicationLoop::CommunicationLoop(std::shared_ptr<Tourist> tourist, std::shared_ptr<SystemInfo> sys_info) {
    this->tourist = tourist;
    this->sys_info = sys_info;
    run_flag = true;
}

CommunicationLoop::~CommunicationLoop() {

}

void CommunicationLoop::run() {
    while (run_flag) {
        packet.receive(*tourist);
        auto msg_type = packet.get_message_type();
        if (msg_type < Packet::REQ_PONY || msg_type > Packet::KILL_MESSAGE) {
            handler_wrong_message();
        } else {
            auto func = lookup[static_cast<int>(msg_type)];
            (this->*func)();
        }
    }
}

void CommunicationLoop::handler_req_pony() {
    int sender_id = packet.get_sender_id();
    tourist->state.mutex_lock();
    switch (tourist->state.unsafe_get()) {
    case Tourist::RESTING:
    case Tourist::ON_SHORE:
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received REQ_PONY from %d, answering ACK_PONY", sender_id);
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Packet(Packet::ACK_PONY).send(*tourist, sender_id);
        break;
    case Tourist::WAIT_PONY:
        if (QueuesSubmarine::is_lower_priority({tourist->get_id(), tourist->my_req_pony_timestamp}, {sender_id, packet.get_timestamp()})) {
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprintf(*tourist, "Received REQ_PONY from %d, answering ACK_PONY", sender_id);
            #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Packet(Packet::ACK_PONY).send(*tourist, sender_id);
        } else {
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprintf(*tourist, "Received REQ_PONY from %d, adding him to my queue pony", sender_id);
            #endif
            tourist->add_to_queue_pony(sender_id);
        }
        break;
    case Tourist::CHOOSE_SUBMAR:
    case Tourist::WAIT_SUBMAR:
    case Tourist::BOARDED:
    case Tourist::TRAVEL:
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received REQ_PONY from %d, adding him to my queue pony", sender_id);
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        tourist->add_to_queue_pony(sender_id);
        break;
    default:
        handler_wrong_state();
        break;
    }
    tourist->state.mutex_unlock();
}

void CommunicationLoop::handler_ack_pony() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::WAIT_PONY) {
        int needed_ack_no = sys_info->get_tourist_no() - sys_info->get_pony_no() + 1;
        if (needed_ack_no <= 1)
            needed_ack_no = 1;
        int received_ack = ++(tourist->received_ack_no);
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received ACK_PONY, I have %d, need %d", received_ack, needed_ack_no);
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        if (received_ack == needed_ack_no && needed_ack_no > 1) {
            tourist->cond_var.notify(ConditionVar::ENOUGH_ACK_SIGNAL);
        }  else {
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprint(*tourist, "Received ACK_PONY, ignoring");
            #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        }
    } else {
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprint(*tourist, "Received ACK_PONY, ignoring");
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    }
}

void CommunicationLoop::handler_req_submar() {
    int sender_id = packet.get_sender_id();
    int submarine_id = packet.get_submarine_id();
    tourist->submarine_queues->safe_add(submarine_id, sender_id, packet.get_timestamp());
    #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    Debug::dprintf(*tourist, "Received REQ_SUBMAR from %d, adding to the queue %d and responding ACK_SUBMAR", sender_id, submarine_id);
    #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    Packet(Packet::ACK_SUBMAR).send(*tourist, sender_id);

    if (tourist->state.safe_get() == Tourist::BOARDED && tourist->is_capitan()) {
        if (tourist->is_submarine_deadlock(*sys_info)) {
            Debug::dprint(*tourist, "Submarine deadlock detected");
            tourist->cond_var.notify(ConditionVar::DEADLOCK_DETECTED_SIGNAL);
        }
    }
}

void CommunicationLoop::handler_ack_submar() {
    if (tourist->state.safe_get() == Tourist::WAIT_SUBMAR) {
        int needed_ack_no = sys_info->get_tourist_no();
        int received_ack = ++(tourist->received_ack_no);
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received ACK_SUBMAR from %d, I have %d, need %d", packet.get_sender_id(), received_ack, needed_ack_no);
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        if (received_ack == needed_ack_no && needed_ack_no > 1) {
            tourist->cond_var.notify(ConditionVar::ALL_ACK_SIGNAL);
        }
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_full_submar_stay() {
    int submarine_id = packet.get_submarine_id();
    #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    Debug::dprintf(*tourist, "Received FULL_SUBMAR_STAY from %d, marking submarine %d as unavailable", packet.get_sender_id(), submarine_id);
    #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    tourist->available_submarine_list.safe_set_element(submarine_id, false);
    if (submarine_id == tourist->my_submarine_id.load()) {
        if (tourist->state.safe_get() == Tourist::BOARDED && tourist->is_capitan()) {
            tourist->cond_var.notify(ConditionVar::SUBMARINE_FULL_SIGNAL);
        }
    }
}

void CommunicationLoop::handler_full_submar_retreat() {
    int submarine_id = packet.get_submarine_id();
    int sender_id = packet.get_sender_id();
    #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    Debug::dprintf(*tourist, "Received FULL_SUBMAR_RETREAT from %d, removing him from queue and marking submarine %d as unavailable", packet.get_sender_id(), submarine_id);
    #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    tourist->available_submarine_list.safe_set_element(submarine_id, false);
    tourist->submarine_queues->safe_remove_tourist_id(submarine_id, sender_id);
    if (submarine_id == tourist->my_submarine_id.load()) {
        if (tourist->state.safe_get() == Tourist::BOARDED && tourist->is_capitan()) {
            bool res = tourist->boarded_on_my_submarine.safe_remove_val_if_exists(sender_id);
            if (res) {
                if (tourist->received_ack_no.load() == tourist->boarded_on_my_submarine.safe_get_size()) {
                    tourist->cond_var.notify(ConditionVar::ALL_ACK_SIGNAL);
                }
            }
            tourist->cond_var.notify(ConditionVar::SUBMARINE_FULL_SIGNAL);
        }
    }
}

void CommunicationLoop::handler_return_submar() {
    int submarine_id = packet.get_submarine_id();
    int passenger_no = packet.get_passenger_no();
    #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    Debug::dprintf(*tourist, "Received RETURN_SUBMAR{%d} from %d. Removing %d tourists from queue", submarine_id, packet.get_sender_id(), passenger_no);
    #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
    tourist->available_submarine_list.safe_set_element(submarine_id, true);
    tourist->submarine_queues->safe_remove_from_begin(submarine_id, passenger_no);
    tourist->state.mutex_lock();
    auto state = tourist->state.unsafe_get();
    int captain_id;
    if (submarine_id == tourist->my_submarine_id.load()) {
        switch (state) {
        case Tourist::WAIT_SUBMAR:
            tourist->cond_var.notify(ConditionVar::MY_SUBMARINE_RETURNED_SIGNAL);
            break;
        case Tourist::TRAVEL:
            captain_id = packet.get_sender_id();
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprintf(*tourist, "End of journey. Responing ACK_TRAVEL to the captain (%d)", captain_id);
            #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Packet(Packet::ACK_TRAVEL).send(*tourist, captain_id);
            tourist->cond_var.notify(ConditionVar::JOURNEY_END_SIGNAL);
            break;
        case Tourist::BOARDED: 
            /* This can happen if a tourist thinks he's on board but in reality
                the submarine is on a journey without him. So if his submarine returns
                he should check if he became a new captain
            */
            tourist->cond_var.notify(ConditionVar::MY_SUBMARINE_RETURNED_SIGNAL);
            break;
        default:
            break;
        }
    }
    tourist->state.mutex_unlock();
}

void CommunicationLoop::handler_travel_ready() {
    tourist->state.mutex_lock();
    auto state = tourist->state.unsafe_get();
    switch(state) {
    case Tourist::WAIT_SUBMAR:
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received TRAVEL_READY from %d, I'll answer when boarded", packet.get_sender_id());
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        tourist->is_ack_travel_queued.store(tourist->my_submarine_id.load());
        break;
    case Tourist::BOARDED:
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received TRAVEL_READY from %d, answering", packet.get_sender_id());
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
        break;
    default:
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
    tourist->state.mutex_unlock();
}

void CommunicationLoop::handler_ack_travel() {
    auto state = tourist->state.safe_get();
    int needed_acks;
    int received_acks;
    switch(state) {
    case Tourist::BOARDED:
        if (tourist->is_capitan()) {
            needed_acks = tourist->boarded_on_my_submarine.safe_get_size();
            received_acks = ++(tourist->received_ack_no);
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprintf(*tourist, "Received ACK_TRAVEL from %d, I have %d, need %d",
                packet.get_sender_id(),
                received_acks,
                needed_acks
            );
            #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            if (received_acks == needed_acks) {
                tourist->cond_var.notify(ConditionVar::ALL_ACK_SIGNAL);
            }
        } else {
            // Only captain should be able to get this message
            handler_wrong_message();
        }
        break;
    case Tourist::TRAVEL:
        if (tourist->is_capitan()) {
            needed_acks = tourist->boarded_on_my_submarine.safe_get_size();
            received_acks = ++(tourist->received_ack_no);
            #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            Debug::dprintf(*tourist, "Received ACK_TRAVEL from %d, I have %d, need %d",
                packet.get_sender_id(),
                received_acks,
                needed_acks
            );
            #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
            if (received_acks == needed_acks) {
                tourist->cond_var.notify(ConditionVar::ALL_ACK_SIGNAL);
            }
        } else {
            // Only captain should be able to get this message
            handler_wrong_message();
        }
        break;
    default:
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
        break;
    }
}

void CommunicationLoop::handler_depart_submar() {
    if (tourist->state.safe_get() == Tourist::BOARDED) {
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Debug::dprintf(*tourist, "Received DEPART_SUBMAR from the captain %d, answering ACK_TRAVEL", packet.get_sender_id());
        #endif //DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
        tourist->cond_var.notify(ConditionVar::JOURNEY_START_SIGNAL);
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_depart_submar_not_full() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::BOARDED) {
        #ifndef DEBUG_PRINT_ONLY_IMPORTANT_STUFF
        // Debug::dprintf(*tourist, "Received DEPART_SUBMAR_NOT_FULL from the captain %d, marking submarines as unavailable and answering ACK_TRAVEL", packet.get_sender_id());
        Debug::dprintf(*tourist, "Received DEPART_SUBMAR_NOT_FULL from the captain %d, answering ACK_TRAVEL", packet.get_sender_id());
        #endif
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
        // Marking all non-empty submarines as unavailable (but not my own submarine)
        // int my_submarine_id = tourist->my_submarine_id.load();
        // tourist->available_submarine_list.mutex_lock();
        // tourist->submarine_queues->mutex_lock();
        // for (int submarine_id=0; submarine_id<sys_info->get_submarine_no(); submarine_id++) {
        //     if (submarine_id == my_submarine_id) continue;
        //     if (tourist->submarine_queues->unsafe_get_size(submarine_id) > 0)
        //         tourist->available_submarine_list.unsafe_set_element(submarine_id, false);
        // }
        // tourist->available_submarine_list.mutex_unlock();
        // tourist->submarine_queues->mutex_unlock();
        tourist->cond_var.notify(ConditionVar::JOURNEY_START_SIGNAL);
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_wrong_state() {
    Debug::dprintf(*tourist, "Wrong state %d! stopping!", tourist->state.unsafe_get());
    run_flag = false;
}

void CommunicationLoop::handler_wrong_message() {
    Debug::dprintf(*tourist, 
        "Received an incorrect or unexpected message (%d) from %d! Stopping!",
        packet.get_message_type(), packet.get_sender_id());
    Packet(Packet::KILL_MESSAGE).broadcast(*tourist, sys_info->get_tourist_no());
    run_flag = false;
}

void CommunicationLoop::handler_kill_message() {
    Debug::dprintf(*tourist,
        "Received 'kill message' from %d! Stopping!", packet.get_sender_id());
    run_flag = false;
}