//
// Created by Bach Dao.
//

#ifndef SIMPLE_BROKER_SERVER_MESSAGE_HANDLER_H
#define SIMPLE_BROKER_SERVER_MESSAGE_HANDLER_H
#include "msg_broker/block.h"
#include "msg_broker/message_handler.h"
#include "msg_broker/server/server_message_handler.h"
#include "msg_broker/server/server_topic_manager.h"

namespace msg_broker::server {
class server_message_handler : public msg_broker::impl::message_handler {

  bool handle_connect(ProtoMessage &msg);
  bool handle_connect_ack(ProtoMessage &ptrMsg);
  bool handle_disconnect(ProtoMessage &ptrMsg);
  bool handle_topic_subscribe(ProtoMessage &ptrMsg);
  bool handle_topic_unsubscribe(ProtoMessage &ptrMsg);
  bool
  handle_normal_message(ProtoMessage &ptrMsg,
                        std::shared_ptr<msg_broker::impl::block> ptrDataFrame);
  msg_broker::server::server_topic_manager &tpManager_;

public:
  explicit server_message_handler(server_topic_manager &tpm);
  bool handle_message(std::shared_ptr<msg_broker::impl::block>) override;
};
} // namespace msg_broker::server

#endif // SIMPLE_BROKER_SERVER_MESSAGE_HANDLER_H
