//
// Created by Bach Dao.
//

#include "msg_broker/client/client_topic.h"

#include "msg_broker/connection.h"

namespace msg_broker::client {
client_topic::client_topic(
    std::shared_ptr<msg_broker::impl::connection> ptrConn,
    const std::string &tpName)
    : ptrConn_(ptrConn), topicName_(tpName) {}

void client_topic::on_message(
    std::function<void(const std::string &)> handler) {
  msgHandler_ = std::move(handler);
}

bool client_topic::send_message(const std::string &msg) {

  auto ptrBlk = impl::message_util::serialize(
      MessageType::NORMAL, ptrConn_->get_id(), topicName_, msg);
  return ptrConn_->send(std::move(ptrBlk));
}

void client_topic::handle_message(const ProtoMessage &msg) {
  if (msgHandler_) {
    msgHandler_(msg.payload());
  }
}

void client_topic::unsubscribe() {
  auto ptr = impl::message_util::serialize(MessageType::TOPIC_UNSUBSCRIBE,
                                           ptrConn_->get_id(), topicName_, "");
  ptrConn_->send(std::move(ptr));
}

} // namespace msg_broker::client
