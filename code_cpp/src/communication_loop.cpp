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
        // TODO: Replace with a lookup
        // tourist->state.mutex_lock(); // TODO: Should be locked?
        packet.receive(*tourist);
        switch(packet.get_message_type()) {
            case Packet::REQ_PONY:
                handler_req_pony();
                break;
            case Packet::ACK_PONY:
                handler_ack_pony();
                break;
            case Packet::REQ_SUBMAR:
                handler_req_submar();
                break;
            case Packet::ACK_SUBMAR:
                handler_ack_submar();
                break;
            case Packet::FULL_SUBMAR_RETREAT:
                handler_full_submar_retreat();
                break;
            case Packet::FULL_SUBMAR_STAY:
                handler_full_submar_stay();
                break;
            case Packet::RETURN_SUBMAR:
                handler_return_submar();
                break;
            case Packet::TRAVEL_READY:
                handler_travel_ready();
                break;
            case Packet::ACK_TRAVEL:
                handler_ack_travel();
                break;
            case Packet::DEPART_SUBMAR:
                handler_depart_submar();
                break;
            case Packet::DEPART_SUBMAR_NOT_FULL:
                handler_depart_submar_not_full();
                break;
            default:
                handler_wrong_message();
                break;
        }
        // tourist->state.mutex_lock();
    }
}

void CommunicationLoop::handler_req_pony() {
    int sender_id = packet.get_sender_id();
    Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received REQ_PONY from %d", sender_id);
    tourist->state.mutex_lock();
    switch (tourist->state.unsafe_get()) { // TODO: To lock or not to lock
    case Tourist::RESTING:
    case Tourist::ON_SHORE:
        Debug::dprintf(Debug::INFO_SENDING, *tourist, "Sending ACK_PONY to %d", sender_id);
        Packet(Packet::ACK_PONY).send(*tourist, sender_id);
        break;
    case Tourist::WAIT_PONY:
        if (tourist->my_req_pony_timestamp.load() > packet.get_timestamp()) {
            Debug::dprintf(Debug::INFO_SENDING, *tourist, "Sending ACK_PONY to %d", sender_id);
            Packet(Packet::ACK_PONY).send(*tourist, sender_id);
        } else {
            Debug::dprintf(Debug::INFO_OTHER, *tourist, "Adding %d to my queue pony", sender_id);
            tourist->queue_pony.safe_push_back(sender_id);
        }
        break;
    case Tourist::CHOOSE_SUBMAR:
    case Tourist::WAIT_SUBMAR:
    case Tourist::BOARDED:
    case Tourist::TRAVEL:
        Debug::dprintf(Debug::INFO_OTHER, *tourist, "Adding %d to my queue pony", sender_id);
        tourist->queue_pony.safe_push_back(sender_id);
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
        auto cond_lock = tourist->ack_pony_condition.mutex_lock();
        int needed_ack_no = sys_info->get_tourist_no() - sys_info->get_pony_no();
        if (needed_ack_no < 0)
            needed_ack_no = 0;
        int received_ack = tourist->increment_received_ack_no();
        Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received ACK_PONY, I have %d, need %d", received_ack, needed_ack_no);
        if (received_ack >= needed_ack_no) {
            tourist->clear_received_ack_no();
            tourist->ack_pony_condition.cond_var.notify_one();
        }  else {
            cond_lock.unlock();
        }
    } else {
        Debug::dprint(Debug::INFO_RECEIVING, *tourist, "Received ACK_PONY, ignoring");
    }
}

void CommunicationLoop::handler_req_submar() {
    int sender_id = packet.get_sender_id();
    int submarine_id = packet.get_submarine_id();
    Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received REQ_SUBMAR from %d, adding to the queue %d and responding ACK_SUBMAR", sender_id, submarine_id);
    tourist->submarine_queues->safe_push_back(submarine_id, sender_id);
    Packet(Packet::ACK_SUBMAR).send(*tourist, sender_id);
}

void CommunicationLoop::handler_ack_submar() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::WAIT_SUBMAR) {
        auto cond_lock = tourist->ack_submar_condition.mutex_lock();
        int needed_ack_no = sys_info->get_tourist_no();
        int received_ack = tourist->increment_received_ack_no();
        Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received ACK_SUBMAR, I have %d, need %d", received_ack, needed_ack_no);
        if (received_ack >= needed_ack_no) {
            tourist->clear_received_ack_no();
            tourist->ack_submar_condition.cond_var.notify_one();
        }  else {
            cond_lock.unlock();
        }
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_full_submar_stay() {
    int submarine_id = packet.get_submarine_id();
    Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received FULL_SUBMAR_STAY from %d, marking submarine %d as unavailable", packet.get_sender_id(), submarine_id);
    tourist->available_submarine_list.safe_set_element(submarine_id, false);
    if (tourist->state.safe_get() == Tourist::BOARDED && tourist->is_capitan()) {
        auto cond_lock = tourist->full_submarine_condition.mutex_lock();
        tourist->full_submarine_condition.cond_var.notify_one();
    }
}

void CommunicationLoop::handler_full_submar_retreat() {
    int submarine_id = packet.get_submarine_id();
    int sender_id = packet.get_sender_id();
    Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received FULL_SUBMAR_RETREAT from %d, removing him from queue and marking submarine %d as unavailable", packet.get_sender_id(), submarine_id);
    tourist->available_submarine_list.safe_set_element(submarine_id, false);
    tourist->submarine_queues->safe_remove_tourist_id(submarine_id, sender_id);
    if (tourist->state.safe_get() == Tourist::BOARDED && tourist->is_capitan()) {
        auto cond_lock = tourist->full_submarine_condition.mutex_lock();
        tourist->full_submarine_condition.cond_var.notify_one();
    }
}

void CommunicationLoop::handler_return_submar() {
    int submarine_id = packet.get_submarine_id();
    int passenger_no = packet.get_passenger_no();
    Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received RETURN_SUBMAR{%d} from %d.", submarine_id, packet.get_sender_id());
    tourist->available_submarine_list.safe_set_element(submarine_id, true);
    tourist->submarine_queues->safe_remove_from_begin(submarine_id, passenger_no);
    tourist->state.mutex_lock();
    auto state = tourist->state.unsafe_get();
    if (state == Tourist::CHOOSE_SUBMAR || state == Tourist::TRAVEL) {
        // TODO: If in CHOOSE_SUMBAR, he should choose this submarine
        auto cond_lock = tourist->submarine_return_condition.mutex_lock();
        tourist->submarine_return_condition.cond_var.notify_one();
    }
    tourist->state.mutex_unlock();
}

void CommunicationLoop::handler_travel_ready() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::WAIT_SUBMAR) {
        Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received TRAVEL_READY from %d, I'll answer when boarded", packet.get_sender_id());
        tourist->queue_ack_travel();
    } else if (state == Tourist::BOARDED) {
        Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received TRAVEL_READY from %d, answering", packet.get_sender_id());
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_ack_travel() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::BOARDED) {
        if (tourist->is_capitan()) {
            auto cond_lock = tourist->ack_travel_condition.mutex_lock();
            int needed_acks = tourist->get_boarded_on_my_submarine_size() - 1; // Don't need my own ack
            int received_acks = tourist->increment_received_ack_no();
            Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received ACK_TRAVEL from %d, I have %d, need %d",
                packet.get_sender_id(),
                received_acks,
                needed_acks
            );
            if (received_acks == needed_acks) {
                tourist->clear_received_ack_no();
                tourist->ack_travel_condition.cond_var.notify_one();
            } else {
                cond_lock.unlock();
            }
        } else {
            // Only captain should be able to get this message
            handler_wrong_message();
        }
    } else if (state == Tourist::TRAVEL) {
        if (tourist->is_capitan()) {
            auto cond_lock = tourist->ack_travel_condition.mutex_lock();
            int needed_acks = tourist->get_boarded_on_my_submarine_size() - 1; // Don't need my own ack
            int received_acks = tourist->increment_received_ack_no();
            Debug::dprintf(Debug::INFO_RECEIVING, *tourist, "Received ACK_TRAVEL from %d, I have %d, need %d",
                packet.get_sender_id(),
                received_acks,
                needed_acks
            );
            if (received_acks == needed_acks) {
                tourist->clear_received_ack_no();
                tourist->ack_travel_condition.cond_var.notify_one();
            } else {
                cond_lock.unlock();
            }
        } else {
            // Only captain should be able to get this message
            handler_wrong_message();
        }
    } else {
        // This should be impossible, so if we reach this, something went wrong
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_depart_submar() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::BOARDED) {
        auto cond_lock = tourist->travel_condition.mutex_lock();
        Debug::dprintf(Debug::INFO_RECEIVING_SENDING, *tourist, "Received DEPART_SUBMAR from the captain, answering ACK_TRAVEL");
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
        tourist->travel_condition.cond_var.notify_one();
    } else {
        // This should be impossible, so if we reach this, something went wrong
        // resting, wait_pony, choose_submar, wait_submar
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_depart_submar_not_full() {
    auto state = tourist->state.safe_get();
    if (state == Tourist::BOARDED) {
        auto cond_lock = tourist->travel_condition.mutex_lock();
        Debug::dprintf(Debug::INFO_RECEIVING_SENDING, *tourist, "Received DEPART_SUBMAR from the captain, answering ACK_TRAVEL");
        Packet(Packet::ACK_TRAVEL).send(*tourist, packet.get_sender_id());
        // Marking all non-empty submarines as unavailable (without my own submarine)
        int my_submarine_id = tourist->my_submarine_id.load();
        tourist->available_submarine_list.mutex_lock();
        tourist->submarine_queues->mutex_lock();
        for (int submarine_id=0; submarine_id<sys_info->get_submarine_no(); submarine_id++) {
            if (submarine_id == my_submarine_id) continue;
            if (tourist->submarine_queues->unsafe_get_size(submarine_id) > 0)
                tourist->available_submarine_list.unsafe_set_element(submarine_id, false);
        }
        tourist->available_submarine_list.mutex_unlock();
        tourist->submarine_queues->mutex_unlock();
        tourist->travel_condition.cond_var.notify_one();
    } else {
        // This should be impossible, so if we reach this, something went wrong
        // resting, wait_pony, choose_submar, wait_submar
        handler_wrong_message();
    }
}

void CommunicationLoop::handler_wrong_state() {
    Debug::dprintf(Debug::ERROR_OTHER, *tourist, "Wrong state %d! stopping!", tourist->state.unsafe_get());
    run_flag = false;
}

void CommunicationLoop::handler_wrong_message() {
    Debug::dprintf(Debug::ERROR_RECEIVING, *tourist, 
        "Received an incorrect or unexpected message (%) from %d! Stopping!",
        packet.get_message_type(), packet.get_sender_id());
    run_flag = false;
}
