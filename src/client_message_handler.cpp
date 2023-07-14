//
// Created by Bach Dao.
//

#include "msg_broker/client/client_message_handler.h"

#include "message.pb.h"

namespace msg_broker {
namespace client {

client_message_handler::client_message_handler() {}

bool client_message_handler::handle_message(
    std::shared_ptr<impl::block> dataFrame) {
  ProtoMessage msg;
  msg.ParseFromArray(dataFrame->data() + sizeof(uint32_t),
                     dataFrame->size() - sizeof(uint32_t));
  auto msgType = msg.type();
  switch (msgType) {
  case CONNECT_ACK:
    ptrConn_->set_state(msg_broker::impl::connection_state::established);
    break;
  case DISCONNECT:
    ptrConn_->terminate();
    break;
  case NORMAL:
    handle_normal_message(std::move(msg));
    break;
  default:
    ptrConn_->terminate();
    return false;
  }
  return true;
}

void client_message_handler::handle_normal_message(ProtoMessage message) {
  auto it = topics_.find(message.topic());
  if (it != topics_.end()) {
    it->second->handle_message(message);
  }
}

void client_message_handler::update_topic_list(
    std::string topicName, std::shared_ptr<client_topic> ptrTp) {
  topics_.insert_or_assign(topicName, ptrTp);
}
} // namespace client
} // namespace msg_broker