//
// Created by Bach Dao.
//

#include "msg_broker/server/server_message_handler.h"

#include "msg_broker/connection.h"
#include "msg_broker/message_util.h"

namespace msg_broker::server {
bool server_message_handler::handle_connect_ack(ProtoMessage &msg) {
  // client is not allowed to send CONNECT_ACK
  return false;
}

bool server_message_handler::handle_disconnect(ProtoMessage &msg) {
  return false;
}

bool server_message_handler::handle_topic_subscribe(ProtoMessage &msg) {
  assert(msg.has_topic());
  auto topicName = msg.topic();
  tpManager_.get_topic(topicName).subscribe(ptrConn_->shared_from_this());
  return true;
}

bool server_message_handler::handle_topic_unsubscribe(ProtoMessage &msg) {
  assert(msg.has_topic());
  auto topicName = msg.topic();

  // unsubscribe from a random server_topic is not allowed
  if (!tpManager_.exist(topicName)) {
    return false;
  }
  tpManager_.get_topic(topicName).unsubscribe(ptrConn_->shared_from_this());
  return true;
}

bool server_message_handler::handle_normal_message(
    ProtoMessage &msg, std::shared_ptr<msg_broker::impl::block> ptrDataFrame) {
  const auto &topicName = msg.topic();

  // server_topic must exist
  if (!tpManager_.exist(topicName)) {
    return false;
  }
  auto &tp = tpManager_.get_topic(topicName);
  if (tp.contains(ptrConn_->get_id())) {
    tp.dispatch(ptrConn_->get_id(), std::move(ptrDataFrame));
    return true;
  }
  return false;
}

bool server_message_handler::handle_message(
    std::shared_ptr<msg_broker::impl::block> ptrDataFrame) {
  ProtoMessage msg;
  // the frame maybe dispatch to another client, so the LEN_HEADER
  //  is retained
  std::span msgData{
      ptrDataFrame->data() + msg_broker::impl::connection::SIZE_OF_LEN_HEADER,
      ptrDataFrame->size() - msg_broker::impl::connection::SIZE_OF_LEN_HEADER};
  msg.ParseFromArray(msgData.data(), msgData.size());

  switch (msg.type()) {
  case CONNECT:
    return handle_connect(msg);
  case CONNECT_ACK:
    return handle_connect_ack(msg);
  case DISCONNECT:
    return handle_disconnect(msg);
  case TOPIC_SUBSCRIBE:
    return handle_topic_subscribe(msg);
  case TOPIC_UNSUBSCRIBE:
    return handle_topic_unsubscribe(msg);
  case NORMAL:
    return handle_normal_message(msg, ptrDataFrame);
  default:
    // should not reach here
    throw std::logic_error("invalid message type");
  }
}

bool server_message_handler::handle_connect(ProtoMessage &msg) {
  ptrConn_->set_id(msg.client_id());

  ptrConn_->send(msg_broker::impl::message_util::get_connect_ack_message(
      ptrConn_->get_id()));

  ptrConn_->set_state(msg_broker::impl::connection_state::established);
  return true;
}

server_message_handler::server_message_handler(server_topic_manager &tpm)
    : tpManager_(tpm) {}

} // namespace msg_broker::server